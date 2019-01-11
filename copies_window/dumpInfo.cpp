#include "dumpInfo.h"
#include "ui_dumpInfo.h"

dumpInfo::dumpInfo(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DumpInfo)
{
	ui->setupUi(this);
	
	connect(ui->dirButton, &QPushButton::clicked, this, &dumpInfo::select_dir);
}

void dumpInfo::select_dir() {
	QString dir = QFileDialog::getExistingDirectory(this, tr("Select Directory for Scanning"),
		QString(),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	ui->lineEdit->setText(dir);
}

dumpInfo::~dumpInfo() = default;
