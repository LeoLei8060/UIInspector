cmake_minimum_required(VERSION 3.15)
project(UIInspector)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(UIInspector
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/uiainspector.cpp
    src/uiainspector.h
)

target_link_libraries(UIInspector PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    UIAutomationCore
)
