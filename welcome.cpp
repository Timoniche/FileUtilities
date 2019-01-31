#include "welcome.h"
#include "ui_welcome.h"

#include "substring_window/mainwindow2.h"
#include "copies_window/mainwindow.h"

#include <memory>

#include <QCommonStyle>
#include <QDesktopWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGuiApplication>
#include <QScreen>

welcome_window::welcome_window(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::welcomeWindow) {
    ui->setupUi(this);
    setWindowTitle("Welcome");
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            qApp->desktop()->availableGeometry()
        )
    );
    setFocusPolicy(Qt::ClickFocus);
    QPixmap bkgnd(":/images/welcome.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);
    this->setFixedSize(this->width(), this->height());
    connect(ui->pushButton, &QPushButton::clicked, this, &welcome_window::go);
    connect(this, SIGNAL(enter_pressed()), this, SLOT(go()));
}

welcome_window::~welcome_window() = default;

void welcome_window::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        emit enter_pressed();
    }
}

void welcome_window::go() {
    int curIndex = ui->comboBox->currentIndex();
    if (curIndex == 0) {
        auto *w = new main_window();
        w->show();
    } else {
        auto *f = new subStringFinder();
        f->show();
    }
}













