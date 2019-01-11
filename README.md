FileUtilities — программа для нахождения дубликатов в директории и не только
-------------------------------------------------------

### Поддерживает функции:
 - удаление всех копий
 - поиск дубликатов конкретного файла
 - поиск подстроки в файле
###
 Тесты (сгенерировать большой 10GB файл):
 - fallocate -l 10G temp_10GB_file (Linux)
 - fsutil file createnew temp_10GB_file 10000000000 (Windows)
 - mkfile -n 10g temp_10GB_file (Mac OS)
 
### Сборка `cmake`'ом (!) 

нужно заменить:
//#include "ui_MainWindow1.h"
#include "substring_window/ui_MainWindow1.h"

    $ cmake -DCMAKE_BUILD_TYPE=Debug .
    $ make

Вместо `-DCMAKE_BUILD_TYPE=Debug` может быть `-DCMAKE_BUILD_TYPE=RelWithDebInfo` или что-то другое в зависимости от того, какую конфигурацию вы желаете.

### Сборка `qmake`'ом

    $ qmake CONFIG+=debug -o Makefile FileCopies.pro
    $ make
