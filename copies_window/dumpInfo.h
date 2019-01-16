#pragma once

#include <QDialog>
#include <memory>
#include <QFileDialog>

namespace Ui {
    class DumpInfo;
}

class dumpInfo : public QDialog {
Q_OBJECT

    friend class main_window;

private slots:

    void select_dir();

public:
    explicit dumpInfo(QWidget *parent = nullptr);

    ~dumpInfo() override;

private:
    std::unique_ptr<Ui::DumpInfo> ui;
};