cmake_minimum_required(VERSION 3.21)

project(KeyboardCalculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS Core Widgets Test)

add_library(calculator_core STATIC
    src/calculator.cpp
    src/calculator.h
)
target_include_directories(calculator_core PUBLIC src)
target_link_libraries(calculator_core PUBLIC Qt6::Core)

add_executable(keyboard_calculator WIN32
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    resources.qrc
)
target_link_libraries(keyboard_calculator PRIVATE calculator_core Qt6::Widgets)

enable_testing()

add_executable(calculator_tests
    tests/test_calculator.cpp
)
target_link_libraries(calculator_tests PRIVATE calculator_core Qt6::Test)
add_test(NAME calculator_tests COMMAND calculator_tests)

add_executable(keyboard_tests
    tests/test_keyboard.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    resources.qrc
)
target_link_libraries(keyboard_tests PRIVATE calculator_core Qt6::Widgets Qt6::Test)
add_test(NAME keyboard_tests COMMAND keyboard_tests)
