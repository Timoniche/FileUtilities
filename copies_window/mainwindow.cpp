#include <memory>

#include "ui_dumpInfo.h"
#include "dumpInfo.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "my_functions.h"
#include "binary_tree.h"

#include <QCommonStyle>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <array>
#include <chrono>
#include <QtCore/QThread>
#include <QTextStream>
#include <QTime>


main_window::main_window(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        dump_window(new dumpInfo(this)) {
    ui->setupUi(this);
    dump_window->setWindowModality(Qt::WindowModality::WindowModal);

    setWindowTitle("File Copies");
    ui->progressBar->setValue(0);
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(),
                                    qApp->desktop()->availableGeometry())); //NOLINT

    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    ui->treeWidget->setUniformRowHeights(true);
    ui->treeWidget->setSelectionMode(QAbstractItemView::MultiSelection);

    QCommonStyle style;
    ui->actionScan_Directory->setIcon(style.standardIcon(QCommonStyle::SP_DialogOpenButton));
    ui->actionExit->setIcon(style.standardIcon(QCommonStyle::SP_DialogCloseButton));
    ui->actionAbout->setIcon(style.standardIcon(QCommonStyle::SP_DialogHelpButton));

    ui->buttonStop->setIcon(style.standardIcon(QCommonStyle::SP_MediaStop));
    ui->continueButton->setIcon(style.standardIcon(QCommonStyle::SP_MediaPlay));
    ui->pauseButton->setIcon(style.standardIcon(QCommonStyle::SP_MediaPause));

    connect(ui->actionScan_Directory, &QAction::triggered, this, &main_window::select_directory);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actionAbout, &QAction::triggered, this, &main_window::show_about_dialog);
    connect(ui->selectButton, &QPushButton::clicked, this, &main_window::select_all_extra_files);
    connect(ui->deleteButton, &QPushButton::clicked, this, &main_window::delete_copies);
    connect(ui->scanButton, &QPushButton::clicked, this, &main_window::scan_directory);
    connect(ui->dumpButton, &QPushButton::clicked, this, &main_window::dump_selected);
    connect(ui->continueButton, &QPushButton::clicked, this, &main_window::restart_thread);
    connect(ui->undoButton, &QPushButton::clicked, this, &main_window::undo_selecting);
    connect(ui->collapse, &QPushButton::clicked, this, &main_window::collapse);
    connect(ui->expand, &QPushButton::clicked, this, &main_window::expand);
    connect(ui->buttonStop, &QPushButton::clicked, this, &main_window::interrupt_thread);
    connect(ui->pauseButton, &QPushButton::clicked, this, &main_window::pause_thread);
    connect(ui->switchButton, &QPushButton::clicked, this, &main_window::switch_widget);
    connect(ui->lineEdit, SIGNAL(textChanged(
                                         const QString &)), this, SLOT(dir_changed(
                                                                               const QString &)));
    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(buttons_control()));

    qRegisterMetaType<MyArray>("MyArray");
    qRegisterMetaType<clock_t>("clock_t");
}

main_window::~main_window() = default;

void main_window::dir_changed(const QString &dir) {
    _cur_dir = dir;
}

void main_window::analyze_log(QString l) {
    auto *item = new QListWidgetItem();
    QTime curr_time = QTime::currentTime();
    item->setText(curr_time.toString("hh:mm:ss") + " log: " + l);
    ui->listWidget->addItem(item);
}

void main_window::buttons_control() {
    if (ui->treeWidget->selectedItems().empty()) {
        ui->undoButton->setEnabled(false);
        ui->dumpButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
    } else {
        ui->undoButton->setEnabled(true);
        ui->dumpButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);
    }
}

void main_window::switch_widget() {
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

void main_window::closeEvent(QCloseEvent *event) {
    interrupt_thread();
    event->accept();
}

void main_window::undo_selecting() {
    ui->treeWidget->clearSelection();
}

void main_window::pause_thread() {
    ui->continueButton->setEnabled(true);
    ui->pauseButton->setEnabled(false);
    ui->buttonStop->setEnabled(false);
    worker->_pause_required = true;
}

void main_window::restart_thread() {
    ui->continueButton->setEnabled(false);
    ui->pauseButton->setEnabled(true);
    ui->buttonStop->setEnabled(true);
    worker->_pause_required = false;
    worker->_pause_Manager.wakeAll();
}

void main_window::error(QString err) {
    auto *item = new QListWidgetItem();
    QTime curr_time = QTime::currentTime();
    item->setText(curr_time.toString("hh:mm:ss") + " err: " + err);
    ui->listWidget->addItem(item);
}

void main_window::analyze_error(QString err) {
    scan_has_finished();
    error(std::move(err));
}

void main_window::select_directory() {

    _cur_dir = QFileDialog::getExistingDirectory(this, tr("Select Directory for Scanning"),
                                                 QString(),
                                                 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->lineEdit->setText(_cur_dir);
}

void main_window::collapse() {
    ui->treeWidget->collapseAll();
}

void main_window::expand() {
    ui->treeWidget->expandAll();
}

void main_window::scan_directory() { //todo: add find file function
    interrupt_thread();

    if (!QDir(_cur_dir).exists()) {
        ui->treeWidget->clear();
        show_message("There is no such file or directory");
    } else {
        ui->treeWidget->clear();
        ui->progressBar->setValue(0);

        ui->buttonStop->setEnabled(true);
        ui->pauseButton->setEnabled(true);
        ui->continueButton->setEnabled(false);
        ui->selectButton->setEnabled(false);
        ui->expand->setEnabled(false);
        ui->collapse->setEnabled(false);

        find_copies(_cur_dir);
    }
}

void main_window::show_message(QString message) {
    auto *item = new QTreeWidgetItem(ui->treeWidget);
    item->setText(0, message);
    ui->treeWidget->addTopLevelItem(item);
}

void main_window::update_bar(int val) {
    ui->progressBar->setValue(ui->progressBar->value() + val);
}

void main_window::merge_pack(MyArray pack) {
    auto *item = new QTreeWidgetItem(ui->treeWidget);

    size_t one_file_size = static_cast<size_t>((new QFileInfo(QString::fromStdString(pack[0][0])))->size());

    size_t item_size = pack.size();
    size_t files_with_size = 0;
    int index_of_child = 1;

    for (size_t i = 0; i < pack.size(); i++) { //NOLINT
        auto *child_class = new QTreeWidgetItem();
        child_class->setText(0, QString::number(pack[i].size()) + QString(tr(" of ")) +
                                QString::number(index_of_child) +
                                tr(my_functions::add_suffix(index_of_child)));
        index_of_child++;
        files_with_size += pack[i].size();
        for (size_t j = 0; j < pack[i].size(); j++) { //NOLINT
            auto *path_of_class = new QTreeWidgetItem();
            path_of_class->setText(0, QString::fromStdString(pack[i][j]));
            child_class->addChild(path_of_class);
        }
        child_class->setText(1, QString::number(one_file_size * pack[i].size()) + QString(tr(" bytes")));
        item->addChild(child_class);
    }

    item->setText(0, QString(tr("Found ")) + QString::number(item_size) +
                     QString(tr(" kind of ")) + QString::number(one_file_size) +
                     QString(tr(" bytes duplicates")));
    size_t extra_space = one_file_size * files_with_size;
    item->setText(1, QString::number(extra_space) + QString(tr(" bytes")));
    _free_space += extra_space;
    ui->treeWidget->addTopLevelItem(item);
}

void main_window::save_max_bar(int val) {
    ui->progressBar->setMaximum(val);
}

void main_window::interrupt_thread() {
    if (thread != nullptr && _isRunning) {
        thread->requestInterruption();
    }
    _isRunning = false;
}

void main_window::find_copies(QString const &dir) {

    _free_space = 0;

    ui->treeWidget->clear();

    _isRunning = true;
    ui->statusBar->showMessage(tr("Searching for copies..."));
    _timeIn = clock();
    thread = new QThread;
    worker = new copies_finder(dir);
    worker->moveToThread(thread);
    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, SIGNAL(finished()), this, SLOT(scan_has_finished()));
    connect(worker, SIGNAL(set_max_bar(int)), this, SLOT(save_max_bar(int)));
    connect(worker, SIGNAL(increase_bar(int)), this, SLOT(update_bar(int)));
    connect(worker, SIGNAL(update_tree(MyArray)), this, SLOT(merge_pack(MyArray)));
    connect(worker, SIGNAL(error(QString)), this, SLOT(analyze_error(QString)));
    connect(worker, SIGNAL(log(QString)), this, SLOT(analyze_log(QString)));

    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

void main_window::dump_selected() {
    if (ui->treeWidget->selectedItems().isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText("No chosen files");
        msgBox.exec();
    } else {
        try {
            if (dump_window->exec() == QDialog::Accepted) {
                _dir_to_dump = dump_window->ui->lineEdit->text();
                if (_dir_to_dump != "") {
                    if (!QDir(_dir_to_dump).exists()) {
                        show_message("There is no such file or directory");
                    } else {
                        QString path_to_file = _dir_to_dump + _file_dump_name;
                        QFile file(path_to_file);
                        if (file.open(QIODevice::WriteOnly)) {
                            QTextStream stream(&file);
                            for (auto u : ui->treeWidget->selectedItems()) {
                                stream << u->text(0) << "\r\n";
                            }
                        } else {
                            throw std::runtime_error("can't make file in this directory");
                        }
                    }
                } else {
                    show_message("empty directory");
                }
            }
        }
        catch (std::exception &ex) {
            error(ex.what());
        }
    }
}

void main_window::scan_has_finished() {
    _isRunning = false;

    ui->selectButton->setEnabled(true);
    ui->buttonStop->setEnabled(false);
    ui->collapse->setEnabled(true);
    ui->expand->setEnabled(true);
    ui->pauseButton->setEnabled(false);
    ui->continueButton->setEnabled(false);

    if (ui->treeWidget->topLevelItemCount() == 0) {
        show_message(QString(tr("no copies here or there were errors \ncheck out log to see more information")));
    }
    clock_t timeOut = clock();
    double timeSpent = ((timeOut - _timeIn) * 1000) / CLOCKS_PER_SEC; //NOLINT

    //TODO: add like QString.args
    ui->statusBar->showMessage(tr("Time spent ") + QString::number(timeSpent) + tr("ms") +
                               " - " + QString::number(timeSpent / 1000) + tr("sec  ") +
                               "|  Free space after deleting " + QString::number(_free_space) + tr(" bytes") +
                               tr(" or ") + QString::number(_free_space / 1000000) + "Mb ");
}

void main_window::select_all_extra_files() {
    //make with invisible root, try to speed up
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
        auto *top_item = ui->treeWidget->topLevelItem(i);
        for (int j = 0; j < top_item->childCount(); j++) {
            auto *child_item = top_item->child(j);
            for (int m = 1; m < child_item->childCount(); m++) {
                child_item->child(m)->setSelected(true);
            }
        }
    }
}

void main_window::delete_copies() {
    if (ui->treeWidget->selectedItems().isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText("No chosen files");
        msgBox.exec();
    } else {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete copies",
                                                                  "Are you sure? \nFiles will be permanently removed",
                                                                  QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            QVector<QString> paths;
            for (auto u : ui->treeWidget->selectedItems()) {
                paths.push_back(u->text(0));
                u->~QTreeWidgetItem();
            }
            for (int i = 0; i < paths.size(); i++) {
                QFile(paths[i]).remove();
            }
        } else {
            //do nothing
        }
    }
}

void main_window::show_about_dialog() {
    QMessageBox::aboutQt(this);
}













