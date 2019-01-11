#include "welcome.h"
#include "ui_welcome.h"
#include "ui_MainWindow1.h"

#include "substring_window/mainwindow2.h"
#include "copies_window/mainwindow.h"

#include <memory>

#include <QCommonStyle>
#include <QDesktopWidget>

welcome_window::welcome_window(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::welcomeWindow)
{
	ui->setupUi(this);

	setWindowTitle("Welcome");
	setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(),
		qApp->desktop()->availableGeometry()));

	connect(ui->pushButton, &QPushButton::clicked, this, &welcome_window::go);
}

welcome_window::~welcome_window() = default;

void welcome_window::go()
{
	int curIndex = ui->comboBox->currentIndex();
	if (curIndex == 0) {
		main_window* w = new main_window();
		w->show();
	}
	else {
		subStringFinder* f = new subStringFinder();
		f->show();
	}
}













