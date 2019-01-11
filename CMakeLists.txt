cmake_minimum_required(VERSION 3.8)

project(FileCopies)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-3")
set(CMAKE_PREFIX_PATH "/Users/ddulaev/Qt/5.11.2/clang_64/lib/cmake")
#set(CMAKE_PREFIX_PATH "C:/Qt/5.12.0/winrt_x86_msvc2017/bin")


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})

add_executable(FileCopies
        main.cpp
        mainwindow.h
        mainwindow.cpp
        mainwindow.ui
        my_functions.h
        my_functions.cpp
        binary_tree.cpp
        binary_tree.h
        copies_finder.cpp
        copies_finder.h
        dumpInfo.cpp
        dumpInfo.h
        dumpInfo.ui
        Boyer_Moore.h
        Boyer_Moore.cpp
        DialogButtonBottom.ui
        file_trigram_finder.h
        file_trigram_finder.cpp
        filtersDialog.ui
        filtersDialog.h
        filtersDialog.cpp
        substring_finder.h
        substring_finder.cpp
        trigram.h
        trigram.cpp
        welcome.ui
        welcome.h
        welcome.cpp
        MainWindow1.ui
        mainwindow2.cpp
        mainwindow2.h
        resources.qrc
        )

target_link_libraries(FileCopies Qt5::Core)
target_link_libraries(FileCopies Qt5::Widgets)
target_link_libraries(FileCopies stdc++fs)

qt5_use_modules(FileCopies Widgets)
