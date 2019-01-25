#include "mainwindow2.h"
#include "ui_MainWindow1.h"

#include "filtersDialog.h"
#include "ui_filtersDialog.h"

#include "Boyer_Moore.h"

#include <QDirIterator>
#include <QThread>
#include <QCheckBox>

#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>
#include <QtConcurrent/QtConcurrentRun>
#include <QtConcurrent/QtConcurrentMap>
#include "my_functions.h"
#include <functional>
#include <QCommonStyle>
//#include <boost/tokenizer.hpp>



subStringFinder::subStringFinder(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow1),
        filtersWindow(new filtersDialog(this)),
        fsWatcher(new QFileSystemWatcher(this)) {

    ui->setupUi(this);
    filtersWindow->setWindowModality(Qt::WindowModality::WindowModal);
    setWindowTitle("substring finder");

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    //ui->splitter->setStretchFactor(0, 2);
    //ui->splitter_2->setStretchFactor(1, 2);
    ui->progressBar->setValue(0);
    ui->indexBar->setValue(0);
    ui->treeWidget->setUniformRowHeights(true);
    ui->treeWidget->setSelectionMode(QAbstractItemView::MultiSelection);

    ui->fileList->setSelectionMode(QAbstractItemView::MultiSelection);

    QCommonStyle style;
    ui->stopButton->setIcon(style.standardIcon(QCommonStyle::SP_MediaStop));
    ui->continueButton->setIcon(style.standardIcon(QCommonStyle::SP_MediaPlay));
    ui->pauseButton->setIcon(style.standardIcon(QCommonStyle::SP_MediaPause));
    ui->action_open_dir->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->action_open_file->setIcon(style.standardIcon(QCommonStyle::SP_FileIcon));

    connect(ui->action_open_file, &QAction::triggered, this, &subStringFinder::add_path);
    connect(ui->searchButton, &QPushButton::clicked, this, &subStringFinder::search);
    connect(ui->action_open_dir, &QAction::triggered, this, &subStringFinder::add_dir);
    connect(ui->expandButton, &QPushButton::clicked, this, &subStringFinder::expand);
    connect(ui->collapseButton, &QPushButton::clicked, this, &subStringFinder::collapse);
    connect(ui->removeButton, &QPushButton::clicked, this, &subStringFinder::remove_selected);
    //write which thread
    connect(ui->pauseButton, &QPushButton::clicked, this, &subStringFinder::pause_thread);
    connect(ui->continueButton, &QPushButton::clicked, this, &subStringFinder::restart_thread);
    connect(ui->stopButton, &QPushButton::clicked, this, &subStringFinder::interrupt_thread);

    connect(ui->actionchoose_filters, &QAction::triggered, this, &subStringFinder::show_filters);

    connect(ui->lineEdit, SIGNAL(textChanged(
                                         const QString &)), this, SLOT(pattern_updated(
                                                                               const QString &)));

    connect(ui->undoButton, &QPushButton::clicked, this, &subStringFinder::undo_selecting);
    connect(ui->switchButton, &QPushButton::clicked, this, &subStringFinder::switch_widget);
    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(buttons_control()));
    connect(ui->fileList, SIGNAL(itemSelectionChanged()), this, SLOT(list_buttons_control()));
    connect(ui->indexStopButton, SIGNAL(clicked()), this, SLOT(stop_indexing()));
    connect(this, SIGNAL(to_log(QString)), this, SLOT(analyze_log(QString)));
    connect(ui->clearLogButton, SIGNAL(clicked()), this, SLOT(clear_log()));
    connect(ui->logDumpButton, SIGNAL(clicked()), this, SLOT(dump_log()));

    connect(this, SIGNAL(indexing_finishing(QString)), this, SLOT(indexing_has_finished(QString)));
    connect(this, SIGNAL(increase_index_bar(int)), this, SLOT(update_increase_bar(int)));
    connect(this, SIGNAL(send_max_index_bar(int)), this, SLOT(set_max_index_bar(int)));


    qRegisterMetaType<MyArray2>("MyArray2");
    qRegisterMetaType<std::string>("std::string");
    qRegisterMetaType<std::set<FilesTrigram, FilesTrigram::cmp>>("std::set<FilesTrigram, FilesTrigram::cmp>");

    connect(fsWatcher.get(), SIGNAL(fileChanged(QString)), this, SLOT(changed(QString)));

    show_filters();
}

void subStringFinder::dump_log() {
    //
}

void subStringFinder::clear_log() {
    ui->listWidget->clear();
}

void subStringFinder::analyze_log(QString l) {
    auto *item = new QListWidgetItem();
    QTime curr_time = QTime::currentTime();
    item->setText(curr_time.toString("hh:mm:ss") + " log: " + l);
    ui->listWidget->addItem(item);
}

void subStringFinder::set_max_index_bar(int val) {
    ui->indexBar->setMaximum(val);
}

void subStringFinder::update_increase_bar(int val) {
    ui->indexBar->setValue(ui->indexBar->value() + val);
}

void subStringFinder::stop_indexing() {
    if (is_indexing) {
        is_indexing = false;
        emit to_log("Indexing interrupted");
        index_map.cancel();
        future.waitForFinished();
        tmp_trigram_list.clear();
        ui->indexStopButton->setEnabled(false);
    }
}

void subStringFinder::list_buttons_control() {
    ui->removeButton->setEnabled(!ui->fileList->selectedItems().empty());
}

void subStringFinder::undo_selecting() {
    ui->treeWidget->clearSelection();
}

void subStringFinder::switch_widget() {
    int index = (ui->stackedWidget->currentIndex() + 1) % 2;
    ui->stackedWidget->setCurrentIndex(index);
    switch (index) {
        case 0:
            ui->switchButton->setText("Go to Log and Errors");
            break;
        case 1:
            ui->switchButton->setText("Go to duplicates tree");
            break;
        default:;
    }
}

void subStringFinder::buttons_control() {
    if (ui->treeWidget->selectedItems().empty()) {
        ui->undoButton->setEnabled(false);
        ui->dumpButton->setEnabled(false);
    } else {
        ui->undoButton->setEnabled(true);
        ui->dumpButton->setEnabled(true);
    }
}

void subStringFinder::closeEvent(QCloseEvent *event) {
    interrupt_thread();
    stop_indexing();
    event->accept();
}

void subStringFinder::pattern_updated(const QString &) {
    interrupt_thread();
    search();
}

void subStringFinder::reload_files() {
    if (!_isRunning) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "reload changed files",
                                                                  "Do you want to reload all files now?",
                                                                  QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            for (auto u : changed_files) {
                for (auto iter = _filesTrigrams.begin(); iter != _filesTrigrams.end(); iter++) {
                    if (iter->get_string_name() == u) {
                        _filesTrigrams.erase(iter);
                        _filesTrigrams.emplace(u);
                        break;
                    }
                }
            }
        } else {
            //do nothing
        }
    }
}

void subStringFinder::changed(const QString &flName) {
    ui->statusBar->showMessage(flName + QString(" changed"));
    changed_files.emplace(flName.toStdString());
    reload_files();
}

void subStringFinder::show_filters() {
    try {
        if (filtersWindow->exec() == QDialog::Accepted) {
            _filters.clear();

            QListIterator<QObject *> i(filtersWindow->ui->groupBox->children());
            while (i.hasNext()) {
                auto *b = qobject_cast<QCheckBox *>(i.next());
                if (b != nullptr && b->isChecked()) {
                    std::string tmp = '*' + b->text().toStdString();
                    _filters << tmp.c_str();
                }
            }
            //add regex here further <?>
            //add list of filters from lineEdit
            std::string list_users_filters = filtersWindow->ui->lineEdit->text().toStdString();
            //char_separator<char> sep(", ");
            std::string own_filter = '*' + list_users_filters;
            _filters << own_filter.c_str();
        }
    }
    catch (std::exception &ex) {
        error(ex.what());
    }
}

void subStringFinder::interrupt_thread() {
    if (thread != nullptr && _isRunning) {
        thread->requestInterruption();
    }
    _isRunning = false;
}

void subStringFinder::update_bar(int val) {
    ui->progressBar->setValue(ui->progressBar->value() + val);
}

void subStringFinder::scan_has_finished() {
    _isRunning = false;
    //ui->selectButton->setEnabled(true);
    ui->stopButton->setEnabled(false);

    ui->collapseButton->setEnabled(true);
    ui->expandButton->setEnabled(true);

    ui->pauseButton->setEnabled(false);
    ui->continueButton->setEnabled(false);

    /*if (ui->treeWidget->topLevelItemCount() == 0) {
        show_message(QString(tr("no copies here")));
    }*/
    clock_t timeOut = clock();
    double timeSpent = ((timeOut - _timeIn) * 1000) / CLOCKS_PER_SEC; //NOLINT

    //TODO: add like QString.args
    ui->statusBar->showMessage(tr("Time spent ") + QString::number(timeSpent) + tr("ms") +
                               " - " + QString::number(timeSpent / 1000) + tr("sec  "));

    if (!changed_files.empty()) {
        reload_files();
    }
}

void subStringFinder::merge_pack(MyArray2 pack) {
    //typedef std::pair<std::string, std::vector< std::pair<int, std::vector<int>> >>  MyArray;
    if (!pack.second.empty()) {
        auto *file_item = new QTreeWidgetItem();
        file_item->setText(0, QString::fromStdString(pack.first));
        size_t matches = 0;
        for (auto in_file : pack.second) {
            auto *item = new QTreeWidgetItem(file_item);
            int row = in_file.first;
            item->setText(0, QString("at ") + QString::number(row) + my_functions::add_suffix(row) + QString(" row"));
            item->setText(1, QString::number(in_file.second.size()) + QString(" matches"));
            for (auto index : in_file.second) {
                auto *item_child = new QTreeWidgetItem(item);
                item_child->setText(0, "pos: " + QString::number(index));
            }
            matches += in_file.second.size();
        }
        file_item->setText(1, QString::number(matches) + QString(" matches"));
        ui->treeWidget->addTopLevelItem(file_item);
    }
}

void subStringFinder::pause_thread() {
    ui->continueButton->setEnabled(true);
    ui->pauseButton->setEnabled(false);
    worker->_pause_required = true;
}

void subStringFinder::restart_thread() {
    ui->continueButton->setEnabled(false);
    ui->pauseButton->setEnabled(true);
    worker->_pause_required = false;
    worker->_pause_Manager.wakeAll();
}

void subStringFinder::error(QString err) {
    auto *item = new QListWidgetItem();
    QTime curr_time = QTime::currentTime();
    item->setText(curr_time.toString("hh:mm:ss") + " err: " + err);
    ui->listWidget->addItem(item);
}

void subStringFinder::analyze_error(QString err) {
    scan_has_finished();
    error(std::move(err));
}

void subStringFinder::remove_selected() {
    for (auto item : ui->fileList->selectedItems()) {
        auto name = item->text();
        if (QDir(name).exists()) {
            QDirIterator it(name, QDir::Hidden | QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext()) {
                auto file_name = it.next();
                fsWatcher->removePath(file_name);
                //make binsearch here
                for (auto iter = _filesTrigrams.begin(); iter != _filesTrigrams.end(); iter++) {
                    if (iter->get_qstring_name() == file_name) {
                        _filesTrigrams.erase(iter);
                        break;
                    }
                }
            }
            delete item;
        } else {
            //make binsearch here
            for (auto iter = _filesTrigrams.begin(); iter != _filesTrigrams.end(); iter++) {
                if (iter->get_qstring_name() == name) {
                    fsWatcher->removePath(iter->get_qstring_name());
                    _filesTrigrams.erase(iter);
                    break;
                }
            }
            //_filesTrigrams.erase(_filesTrigrams.find(name.toStdString()));
            //ask q about finding with other comparator
            delete item;
        }
    }
}

void subStringFinder::expand() {
    ui->treeWidget->expandAll();
}

void subStringFinder::collapse() {
    ui->treeWidget->collapseAll();
}

void subStringFinder::indexing_has_finished(QString dir) {
    if (!is_indexing) {
        return;
    }
    is_indexing = false;
    ui->indexStopButton->setEnabled(false);
    size_t cur_size = _filesTrigrams.size();

    for (auto &u : tmp_trigram_list) {
        if (u.isValid) {
            fsWatcher->addPath(u.get_qstring_name());
            _filesTrigrams.insert(std::move(u));
        }
    }

    size_t last_size = _filesTrigrams.size();

    if (cur_size == last_size) {
        //show message "this file already added or couldn't open"
    } else {
        ui->fileList->insertItem(0, QString("%1").arg(dir));
    }
}

void subStringFinder::start_indexing(QString dir) {
    QDirIterator it(dir, _filters, QDir::Hidden | QDir::Files, QDirIterator::Subdirectories);

    std::set<std::string> files_push_to_trigrams;
    while (it.hasNext()) {
        if (!is_indexing) { return; }
        QString cur_path = it.next();
        if (QFileInfo(cur_path).size() == 0) { continue; }
        files_push_to_trigrams.insert(cur_path.toStdString());
    }
    emit send_max_index_bar(static_cast<int>(files_push_to_trigrams.size()));
    std::function<FilesTrigram(std::string)> f = [this](std::string s) -> FilesTrigram {
        //if error occurs here -> isValid = false;
        FilesTrigram ft(s, &is_indexing);
        emit increase_index_bar(1);
        if (!ft.isValid) {
            emit to_log(QString("Can't open ") + QString::fromStdString(s));
        }
        return ft;
    };

    index_map = QtConcurrent::mapped(files_push_to_trigrams, f);
    tmp_trigram_list = index_map.results();
    emit indexing_finishing(dir);
}

void subStringFinder::add_dir() {
    ui->indexBar->setValue(0);
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory for Scanning"),
                                                    QString(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir == "") { return; }
    ui->indexStopButton->setEnabled(true);

    tmp_trigram_list.clear();
    is_indexing = true;

    future = QtConcurrent::run(this, &subStringFinder::start_indexing, dir);
}

void subStringFinder::add_path() {
    QString path = QFileDialog::getOpenFileName(this, tr("Select file where to find substring"));
    if (path == "") { return; }
    size_t last_size = _filesTrigrams.size();
    try {
        _filesTrigrams.emplace(path.toStdString());
    } catch (std::exception &ex) {
        error(ex.what());
    }
    size_t cur_size = _filesTrigrams.size();
    if (cur_size == last_size) {
        //show message "this file already added or couldn't open"
    } else {
        ui->fileList->insertItem(0, QString("%1").arg(path));
        fsWatcher->addPath(path);
    }

}

void subStringFinder::search() {
    interrupt_thread();
    std::string pattern = ui->lineEdit->text().toStdString();
    if (_filesTrigrams.empty() || pattern.empty()) return;

    ui->statusBar->showMessage(tr("Searching substring..."));
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(static_cast<int>(_filesTrigrams.size()));
    ui->treeWidget->clear();

    ui->pauseButton->setEnabled(true);
    ui->stopButton->setEnabled(true);
    ui->collapseButton->setEnabled(false);
    ui->expandButton->setEnabled(false);

    if (_filesTrigrams.empty()) {
        //show no files message
        return;
    }

    _timeIn = clock();
    thread = new QThread;
    worker = new substring_finder();
    worker->moveToThread(thread);
    _isRunning = true;

    connect(this, &subStringFinder::start_substring_finder, worker, &substring_finder::process);
    //connect(thread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, SIGNAL(finished()), this, SLOT(scan_has_finished()));
    //connect(worker, SIGNAL(set_max_bar(int)), this, SLOT(save_max_bar(int)));
    connect(worker, SIGNAL(increase_bar(int)), this, SLOT(update_bar(int)));
    connect(worker, SIGNAL(update_tree(MyArray2)), this, SLOT(merge_pack(MyArray2)));
    connect(worker, SIGNAL(error(QString)), this, SLOT(analyze_error(QString)));
    connect(worker, SIGNAL(push_to_log(QString)), this, SLOT(analyze_log(QString)));

    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
    emit start_substring_finder(pattern, &_filesTrigrams);
}

subStringFinder::~subStringFinder() = default;















