cmake_minimum_required(VERSION 3.5)

project(SimuladorSO VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR})

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

set(PROJECT_SOURCES
    sources/main.cpp
    sources/janelaPrincipal.cpp
    sources/leitorcsv.cpp
    sources/escalonador.cpp
    sources/gerenciadorMemoria.cpp
    sources/simulador.cpp
    sources/widgetLinhaDoTempo.cpp
    headers/janelaPrincipal.h
    headers/leitorcsv.h
    headers/processo.h
    headers/escalonador.h
    headers/gerenciadorMemoria.h
    headers/simulador.h
    headers/widgetLinhaDoTempo.h
    janelaPrincipal.ui
)

add_executable(SimuladorSO ${PROJECT_SOURCES})

target_include_directories(SimuladorSO PRIVATE headers)

target_link_libraries(SimuladorSO PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)
