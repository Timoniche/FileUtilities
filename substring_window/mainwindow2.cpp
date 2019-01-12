#include "mainwindow2.h"
#include "ui_MainWindow1.h"

#include "filtersDialog.h"
#include "ui_filtersDialog.h"

#include "Boyer_Moore.h"

#include <QDirIterator>
#include <QThread>
#include <QCheckBox>

#include <QMessageBox>
#include "my_functions.h"

//#include <boost/tokenizer.hpp>



subStringFinder::subStringFinder(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow1),
	filtersWindow(new filtersDialog(this))
{

	ui->setupUi(this);
	filtersWindow->setWindowModality(Qt::WindowModality::WindowModal);
	setWindowTitle("substring finder");

	//ui->splitter->setStretchFactor(0, 2);
	//ui->splitter_2->setStretchFactor(1, 2);
	ui->progressBar->setValue(0);
	ui->treeWidget->setUniformRowHeights(true);
	ui->treeWidget->setSelectionMode(QAbstractItemView::MultiSelection);

	ui->fileList->setSelectionMode(QAbstractItemView::MultiSelection);

	connect(ui->addFileButton, &QPushButton::clicked, this, &subStringFinder::add_path);
	connect(ui->searchButton, &QPushButton::clicked, this, &subStringFinder::search);
	connect(ui->addDirButton, &QPushButton::clicked, this, &subStringFinder::add_dir);
	connect(ui->expandButton, &QPushButton::clicked, this, &subStringFinder::expand);
	connect(ui->collapseButton, &QPushButton::clicked, this, &subStringFinder::collapse);
	connect(ui->removeButton, &QPushButton::clicked, this, &subStringFinder::remove_selected);
	//write which thread
	connect(ui->pauseButton, &QPushButton::clicked, this, &subStringFinder::pause_thread);
	connect(ui->continueButton, &QPushButton::clicked, this, &subStringFinder::restart_thread);
	connect(ui->stopButton, &QPushButton::clicked, this, &subStringFinder::interrupt_thread);

	connect(ui->actionchoose_filters, &QAction::triggered, this, &subStringFinder::show_filters);

	connect(ui->lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(pattern_updated(const QString &)));

	connect(ui->undoButton, &QPushButton::clicked, this, &subStringFinder::undo_selecting);

	qRegisterMetaType<MyArray2>("MyArray2");
	qRegisterMetaType<std::string>("std::string");
	qRegisterMetaType<std::set<FilesTrigram, FilesTrigram::cmp>>("std::set<FilesTrigram, FilesTrigram::cmp>");

	fsWatcher = new QFileSystemWatcher(this);
	connect(fsWatcher, SIGNAL(fileChanged(QString)), this, SLOT(changed(QString)));
	show_filters();
}

void subStringFinder::undo_selecting() {
	ui->treeWidget->clearSelection();
}

void subStringFinder::closeEvent(QCloseEvent *event) {
	interrupt_thread();
	event->accept();
}

void subStringFinder::pattern_updated(const QString&) {
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
		}
		else {
			//do nothing
		}
	}
}

void subStringFinder::changed(const QString& flName) {
	ui->statusBar->showMessage(flName + QString(" changed"));
	changed_files.emplace(flName.toStdString());
	reload_files();
}

void subStringFinder::show_filters() {
	try {
		if (filtersWindow->exec() == QDialog::Accepted) {
			_filters.clear();

			QListIterator<QObject *> i(filtersWindow->ui->groupBox->children());
			while (i.hasNext())
			{
				QCheckBox* b = qobject_cast<QCheckBox*>(i.next());
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
	catch (std::exception& ex) {
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
		auto* file_item = new QTreeWidgetItem();
		file_item->setText(0, QString::fromStdString(pack.first));
		for (auto in_file : pack.second) {
			auto* item = new QTreeWidgetItem(file_item); \
			int row = in_file.first;
			item->setText(0, QString("at ") + QString::number(row) + my_functions::add_suffix(row) + QString(" row"));
			for (auto index : in_file.second) {
				auto* item_child = new QTreeWidgetItem(item);
				item_child->setText(0, "pos: " + QString::number(index));
			}
		}
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
	auto *item = new QTreeWidgetItem(ui->treeWidget);
	item->setText(0, err);
	ui->treeWidget->addTopLevelItem(item);
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
		}
		else {
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

void subStringFinder::add_dir() {
	//check if cancelled
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory for Scanning"),
	QString(),
	QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	QDirIterator it(dir, _filters, QDir::Hidden | QDir::Files, QDirIterator::Subdirectories);

	size_t last_size = _filesTrigrams.size();
	while (it.hasNext()) {
		QString cur_path = it.next();
		if (QFileInfo(cur_path).size() == 0) continue;
		try {
			_filesTrigrams.emplace(cur_path.toStdString());
			fsWatcher->addPath(cur_path);
		}
		catch (std::exception& ex) {
			error(ex.what());
		}
	}
	size_t cur_size = _filesTrigrams.size();

	if (cur_size == last_size) {
		//show message "this file already added or couldn't open"
	}
	else {
		ui->fileList->insertItem(0, QString("%1").arg(dir));
	}
}

void subStringFinder::add_path() {
	QString path = QFileDialog::getOpenFileName(this, tr("Select file where to find substring"));
	
	size_t last_size = _filesTrigrams.size();
	try {
		_filesTrigrams.emplace(path.toStdString());
	} catch (std::exception& ex) {
		error(ex.what());
	}
	size_t cur_size = _filesTrigrams.size();
	if (cur_size == last_size) {
		//show message "this file already added or couldn't open"
	}
	else {
		ui->fileList->insertItem(0, QString("%1").arg(path));
		fsWatcher->addPath(path);
	}
	
}

void subStringFinder::search() {
	interrupt_thread();

	ui->statusBar->showMessage(tr("Searching substring..."));
	ui->progressBar->setValue(0);
	ui->progressBar->setMaximum(static_cast<int>(_filesTrigrams.size()));
	ui->treeWidget->clear();

	ui->pauseButton->setEnabled(true);
	ui->stopButton->setEnabled(true);

	std::string pattern = ui->lineEdit->text().toStdString();
	if (pattern == "") {
		//show no message
		return;
	}
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

	connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
	connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

	thread->start();
	emit start_substring_finder(pattern, &_filesTrigrams);
}

subStringFinder::~subStringFinder() = default;















