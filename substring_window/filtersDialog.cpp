#include "filtersDialog.h"
#include "ui_filtersDialog.h"

filtersDialog::filtersDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Filters)
{
	ui->setupUi(this);

}

filtersDialog::~filtersDialog() = default;