#include "copies_window/mainwindow.h"
#include "welcome.h"

#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	welcome_window w;
    //main_window w;
    w.show();

    return QApplication::exec();
}


//whole
//todo: all cons & dest from h files to cpp

//task_1:
//todo: вместо стоп выгружать выбранные в отдельном файле в выбранную боксом директорию
//todo: поиск файла с рута
//todo: режим помощи (подсветка клавиш) с заглавной юишки + при наведении на кнопку инфу о них
//todo: картинки для кнопок
//todo: начальное меню (со второй домкой)
//todo: Бойера-Мура
//todo: добавить паузу
//todo: настройки а-ля relative path на начальном окне
//todo: после эксепшна предложить продолжить с curState
//todo: уменьшить копипасту в дереве
//todo: асинхронность (много потоков для хешей)
//todo: сделать обработчик для "too many streams" увеличив
//первоначальную константу буфера
//todo: добавить мини-игру в левом верхнем углу
//todo: добавить лог в левом нижнем углу с выводом и всех ошибок (типа filed pushed successfully)

//____________________________________________________________________________________________________

//task_2:
//todo: добавить выделение найденных слов прямо в тексте (аналог dump)