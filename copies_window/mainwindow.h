#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dumpInfo.h"
#include "copies_finder.h"

#include <QMainWindow>
#include <memory>

#include <QCloseEvent>

typedef std::vector<std::vector<std::string> > MyArray;


namespace Ui {
    class MainWindow;
}

class main_window : public QMainWindow {
Q_OBJECT

public:
    explicit main_window(QWidget *parent = nullptr);

    ~main_window() override;

private slots:

    void dir_changed(const QString &dir);

    void buttons_control();

    void switch_widget();

    void dump_selected();

    void undo_selecting();

    void expand();

    void collapse();

    void analyze_error(QString err);

    void interrupt_thread();

    void select_directory();

    void scan_directory();

    void show_about_dialog();

    void show_message(QString message);

    void delete_copies();

    void select_all_extra_files();

    void find_copies(QString const &dir);

    void update_bar(int val);

    void merge_pack(MyArray pack);

    void save_max_bar(int val);

    void scan_has_finished();

    void analyze_log(QString l);

protected:
    void closeEvent(QCloseEvent *event) override;

private:

    QString _cur_dir;

    //TODO: change all pointers to unique ptr
    bool _isRunning = false;

    QString _dir_to_dump;

    QString _file_dump_name = "/dump_file.txt";

    clock_t _timeIn{};

    bool _help_mode = false;

    std::unique_ptr<Ui::MainWindow> ui;

    std::unique_ptr<dumpInfo> dump_window;

    void error(QString err);

    QThread *thread = nullptr;

    copies_finder *worker = nullptr;

    void pause_thread();

    void restart_thread();

    size_t _free_space{};
};

#endif // MAINWINDOW_H
