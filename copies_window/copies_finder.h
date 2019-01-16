#include <utility>

#ifndef FILECOPIES_COPIES_FINDER_H
#define FILECOPIES_COPIES_FINDER_H

#include <QObject>
#include <QTreeWidgetItem>
#include <cstddef>
#include <vector>
#include <string>
#include <QVector>
#include <chrono>
#include <QWaitCondition>
#include <QMutex>

typedef std::vector<std::vector<std::string> > MyArray;

class copies_finder : public QObject {
Q_OBJECT

public:
    explicit copies_finder(QString dir) : _dir(std::move(dir)) {}

public slots:

    void process();

signals:

    void increase_bar(int val);

    void set_max_bar(int val);

    void update_tree(MyArray pack);

    void finished();

    void error(QString err);

    void log(QString l);

private:
    QString _dir;

    void process_impl();

//TODO: make it private
public:
    QWaitCondition _pause_Manager;
    QMutex _continue;
    bool _pause_required = false;
};


#endif //FILECOPIES_COPIES_FINDER_H
