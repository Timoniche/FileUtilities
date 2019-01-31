#include "filtersDialog.h"
#include "ui_filtersDialog.h"

#include <QStyle>
#include <QDesktopWidget>
#include <QGuiApplication>
#include <QScreen>

filtersDialog::filtersDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Filters) {
    ui->setupUi(this);
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            qApp->desktop()->availableGeometry())
    );
    connect(ui->chooseAll, SIGNAL(stateChanged(int)), this, SLOT(choose_all(int)));
    connect(ui->resetAll, SIGNAL(stateChanged(int)), this, SLOT(reset_all(int)));
}

void filtersDialog::choose_all(int index) {
    if (index == 2) {
        QListIterator<QObject *> i(ui->groupBox->children());
        while (i.hasNext()) {
            QCheckBox *b = qobject_cast<QCheckBox *>(i.next());
            if (b != nullptr) {
                b->setChecked(true);
            }
        }
        ui->resetAll->setChecked(false);
    }
}

void filtersDialog::reset_all(int index) {
    if (index == 2) {
        QListIterator<QObject *> i(ui->groupBox->children());
        while (i.hasNext()) {
            QCheckBox *b = qobject_cast<QCheckBox *>(i.next());
            if (b != nullptr) {
                b->setChecked(false);
            }
        }
        ui->lineEdit->clear();
        ui->chooseAll->setChecked(false);
    }
}

filtersDialog::~filtersDialog() = default;
