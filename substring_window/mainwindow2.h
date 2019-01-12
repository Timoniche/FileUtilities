#ifndef MAINWINDOW2_H
#define MAINWINDOW2_H

#include "my_functions.h"
#include "trigram.h"

#include "copies_window/dumpInfo.h"
#include "filtersDialog.h"
#include "substring_finder.h"

#include <QMainWindow>
#include <memory>

#include <QFileSystemWatcher>
#include <QCloseEvent>

//name of file & occurrences
typedef std::pair<std::string, std::vector< std::pair<int, std::vector<int>> >>  MyArray2;
//typedef std::string type1;
//typedef std::set<FilesTrigram> type2;

namespace Ui {
	class MainWindow1;
}

class subStringFinder : public QMainWindow {
	Q_OBJECT

public:
	explicit subStringFinder(QWidget *parent = nullptr);

	~subStringFinder() override;
	
signals:
	//void start_substring_finder(std::string const& pattern,
	//	std::set<FilesTrigram, FilesTrigram::cmp> const& _filesTrigrams);
	void start_substring_finder(std::string const& pattern,
		std::set<FilesTrigram, FilesTrigram::cmp> const * _filesTrigrams);

private slots:

	void expand();
	void collapse();
	void remove_selected();
	void add_path();
	void add_dir();
	void search();
	void analyze_error(QString err);
	void scan_has_finished();
	void merge_pack(MyArray2 pack);
	void update_bar(int val);
	void interrupt_thread();
	void show_filters();
	void reload_files();

	//____________________
	void changed(const QString& flName);
	void pattern_updated(const QString &);

protected:
	void closeEvent(QCloseEvent *event) override;

private:
	clock_t _timeIn;
	QStringList _filters;
	bool _isRunning = false;

	std::set<FilesTrigram, FilesTrigram::cmp> _filesTrigrams;
	std::unique_ptr<Ui::MainWindow1> ui;

	QThread* thread = nullptr;
	substring_finder* worker = nullptr;

	std::unique_ptr<filtersDialog> filtersWindow;
	std::set<std::string> changed_files;

	QFileSystemWatcher *fsWatcher;

	void error(QString err);
	void pause_thread();
	void restart_thread();
};

#endif // MAINWINDOW2_H


