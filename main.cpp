#include "copies_window/mainwindow.h"
#include "welcome.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    welcome_window w;
    w.show();

    return QApplication::exec();
}


//whole
//todo: all cons & dest from h files to cpp

//task_1:
//todo: поиск файла с рута
//todo: режим помощи (подсветка клавиш) с заглавной юишки + при наведении на кнопку инфу о них
//todo: настройки relative path на начальном окне
//todo: асинхронность (много потоков для хешей)

//____________________________________________________________________________________________________

//task_2:
//todo: добавить выделение найденных слов прямо в тексте (аналог dump)
