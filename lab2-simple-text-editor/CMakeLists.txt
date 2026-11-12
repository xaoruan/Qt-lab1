cmake_minimum_required(VERSION 3.21)

project(SimpleTextEditor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg)

add_library(lab2_text_core STATIC
    src/fileservice.cpp
    src/fileservice.h
    src/textsearch.cpp
    src/textsearch.h
)
target_include_directories(lab2_text_core PUBLIC src)
target_link_libraries(lab2_text_core PUBLIC Qt${QT_VERSION_MAJOR}::Widgets)

add_executable(simple_text_editor WIN32
    src/aboutdialog.cpp
    src/aboutdialog.h
    src/aboutdialog.ui
    src/codeeditor.cpp
    src/codeeditor.h
    src/finddialog.cpp
    src/finddialog.h
    src/finddialog.ui
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/replacedialog.cpp
    src/replacedialog.h
    src/replacedialog.ui
    resources/lab2.qrc
)
target_link_libraries(simple_text_editor PRIVATE
    lab2_text_core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Svg
)
