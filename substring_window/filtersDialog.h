#pragma once

#include <QDialog>
#include <memory>
#include <QFileDialog>

namespace Ui {
	class Filters;
}

class filtersDialog : public QDialog
{
	Q_OBJECT

		friend class subStringFinder;

private slots:
    void choose_all(int index);
    void reset_all(int index);

public:
	explicit filtersDialog(QWidget *parent = nullptr);
	~filtersDialog() override;

private:
	std::unique_ptr<Ui::Filters> ui;
};
