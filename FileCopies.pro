#-------------------------------------------------
#
# Project created by QtCreator 2018-10-21T16:36:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FileCopies
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#QMAKE_CXXFLAGS += -std=c++17
CONFIG += c++17
QT += concurrent
SOURCES += \
        main.cpp \
        copies_window/mainwindow.cpp \
        my_functions.cpp \
        copies_window/binary_tree.cpp \
        copies_window/copies_finder.cpp \
        copies_window/dumpInfo.cpp \
        substring_window/Boyer_Moore.cpp \
        substring_window/file_trigram_finder.cpp \
        substring_window/filtersDialog.cpp \
        substring_window/substring_finder.cpp \
        substring_window/trigram.cpp \
        welcome.cpp \
        substring_window/mainwindow2.cpp \

HEADERS += \
        copies_window/mainwindow.h \
        my_functions.h \
        copies_window/binary_tree.h \
        copies_window/copies_finder.h \
        copies_window/dumpInfo.h \
        substring_window/Boyer_Moore.h \
        substring_window/file_trigram_finder.h \
        substring_window/filtersDialog.h \
        substring_window/substring_finder.h \
        substring_window/trigram.h \
        welcome.h \
        substring_window/mainwindow2.h

FORMS += \
        copies_window/mainwindow.ui \
        copies_window/dumpInfo.ui \
        copies_window/DialogButtonBottom.ui \
        substring_window/filtersDialog.ui \
        welcome.ui \
        substring_window/MainWindow1.ui 

RESOURCES += \
    resources.qrc


