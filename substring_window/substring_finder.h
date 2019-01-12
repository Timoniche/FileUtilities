#pragma once

#include <utility>
#include <QObject>
#include <QTreeWidgetItem>
#include <cstddef>
#include <vector>
#include <string>
#include <QVector>
#include <chrono>
#include <QWaitCondition>
#include <QMutex>
#include <set>
#include "substring_window/trigram.h"

//name of file & occurrences
typedef std::pair<std::string, std::vector< std::pair<int, std::vector<int>> >>  MyArray2;

class substring_finder : public QObject {
	Q_OBJECT

public:
	substring_finder() = default;
	~substring_finder() = default;

public slots:

	void process(std::string const& pattern, std::set<FilesTrigram, FilesTrigram::cmp> const * files_trigrams);

signals:

	void increase_bar(int val);
	
	void update_tree(MyArray2 pack);

	void finished();

	void error(QString err);

private:

	void process_impl(std::string const& pattern, std::set<FilesTrigram, FilesTrigram::cmp> const * files_trigrams);

	//TODO: make it private
public:
	QWaitCondition _pause_Manager;
	QMutex _continue;
	bool _pause_required = false;
};

