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
find_package(Qt5Concurrent REQUIRED)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Concurrent_INCLUDE_DIRS})


add_executable(FileCopies
        main.cpp
        copies_window/mainwindow.h
        copies_window/mainwindow.cpp
        copies_window/mainwindow.ui
        my_functions.h
        my_functions.cpp
        copies_window/binary_tree.cpp
        copies_window/binary_tree.h
        copies_window/copies_finder.cpp
        copies_window/copies_finder.h
        copies_window/dumpInfo.cpp
        copies_window/dumpInfo.h
        copies_window/dumpInfo.ui
        substring_window/Boyer_Moore.h
        substring_window/Boyer_Moore.cpp
        copies_window/DialogButtonBottom.ui
        substring_window/file_trigram_finder.h
        substring_window/file_trigram_finder.cpp
        substring_window/filtersDialog.ui
        substring_window/filtersDialog.h
        substring_window/filtersDialog.cpp
        substring_window/substring_finder.h
        substring_window/substring_finder.cpp
        substring_window/trigram.h
        substring_window/trigram.cpp
        welcome.ui
        welcome.h
        welcome.cpp
        substring_window/MainWindow1.ui
        substring_window/mainwindow2.cpp
        substring_window/mainwindow2.h
        resources.qrc
        )

target_link_libraries(FileCopies Qt5::Core)
target_link_libraries(FileCopies Qt5::Widgets)
target_link_libraries(FileCopies Qt5::Concurrent)
target_link_libraries(FileCopies stdc++fs)

qt5_use_modules(FileCopies Widgets)
