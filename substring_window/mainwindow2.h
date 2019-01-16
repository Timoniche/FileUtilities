#ifndef MAINWINDOW2_H
#define MAINWINDOW2_H

#include "substring_window/trigram.h"

#include "filtersDialog.h"
#include "substring_finder.h"

#include <QMainWindow>
#include <memory>

#include <QFileSystemWatcher>
#include <QCloseEvent>
#include <QFuture>
#include <QList>

#include <QtConcurrent/QtConcurrent>
#include <QtConcurrent/QtConcurrentRun>
#include <QtConcurrent/QtConcurrentMap>
#include <atomic>

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
	void indexing_finishing(QString dir);
    void increase_index_bar(int val);
    void send_max_index_bar(int val);
    void to_log(QString l);

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
	void undo_selecting();
	void switch_widget();
	void buttons_control();

	//____________________
	void changed(const QString& flName);
	void pattern_updated(const QString &);
	void indexing_has_finished(QString dir);
    void list_buttons_control();
    void stop_indexing();
    void set_max_index_bar(int val);
    void update_increase_bar(int val);
    void clear_log();
    void dump_log();

    void analyze_log(QString l);

protected:
	void closeEvent(QCloseEvent *event) override;

private:
	clock_t _timeIn = 0;
	QStringList _filters;
	bool _isRunning = false;
    std::atomic_bool is_indexing{false};

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

	QFuture<void> future;
	void start_indexing(QString dir);
	QFuture<FilesTrigram> index_map;
	QList<FilesTrigram> tmp_trigram_list;
};

#endif // MAINWINDOW2_H


