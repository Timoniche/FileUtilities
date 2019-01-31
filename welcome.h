#ifndef WELCOME_H
#define WELCOME_H

#include <QWidget>
#include <memory>

namespace Ui {
    class welcomeWindow;
}

class welcome_window : public QWidget {
Q_OBJECT

private slots:

    void go();

signals:
    void enter_pressed();

protected:
    void keyPressEvent(QKeyEvent *event) override;

public:
    explicit welcome_window(QWidget *parent = nullptr);

    ~welcome_window() override;

private:

    std::unique_ptr<Ui::welcomeWindow> ui;

};

#endif // WELCOME_H
