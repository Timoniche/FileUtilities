#include "welcome.h"
#include "ui_welcome.h"

//#include "substring_window/ui_MainWindow1.h"
#include "ui_MainWindow1.h"

#include "substring_window/mainwindow2.h"
#include "copies_window/mainwindow.h"

#include <memory>

#include <QCommonStyle>
#include <QDesktopWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>

welcome_window::welcome_window(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::welcomeWindow) {
    ui->setupUi(this);

    //setStyleSheet("background-image: url(.images/welcome.jpg)");

    setWindowTitle("Welcome");
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(),
                                    qApp->desktop()->availableGeometry()));

    QPixmap bkgnd(":/images/welcome.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);
    this->setFixedSize(this->width(), this->height());
    connect(ui->pushButton, &QPushButton::clicked, this, &welcome_window::go);
}

welcome_window::~welcome_window() = default;

void welcome_window::go() {
    int curIndex = ui->comboBox->currentIndex();
    if (curIndex == 0) {
        main_window *w = new main_window();
        w->show();
    } else {
        subStringFinder *f = new subStringFinder();
        f->show();
    }
}













