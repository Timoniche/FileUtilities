FileUtilities — программа для нахождения дубликатов и подстрок в директории
-------------------------------------------------------

### Поддерживает функции:
 - удаление всех/выбранных копий
 - поиск дубликатов конкретного файла
 - поиск подстроки сразу в нескольких директориях/файлах
 - запись дерева поиска в отдельный файл
###
 Тесты на overflow (сгенерировать большой 10GB файл):
 - fallocate -l 10G temp_10GB_file (Linux)
 - fsutil file createnew temp_10GB_file 10000000000 (Windows)
 - mkfile -n 10g temp_10GB_file (Mac OS)
 
### Сборка `cmake`'ом 

    $ cmake -DCMAKE_BUILD_TYPE=Debug .
    $ make

Вместо `-DCMAKE_BUILD_TYPE=Debug` может быть `-DCMAKE_BUILD_TYPE=RelWithDebInfo` или что-то другое в зависимости от того, какую конфигурацию вы желаете.

### Сборка `qmake`'ом

    $ qmake CONFIG+=debug -o Makefile FileCopies.pro
    $ make
