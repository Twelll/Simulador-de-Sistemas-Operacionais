QT       += core gui widgets

CONFIG   += c++14
DEFINES  += QT_DEPRECATED_WARNINGS

TARGET   = SimuladorSO
TEMPLATE = app

SOURCES += \
    sources/main.cpp \
    sources/janelaPrincipal.cpp \
    sources/leitorcsv.cpp \
    sources/escalonador.cpp \
    sources/gerenciadorMemoria.cpp \
    sources/simulador.cpp \
    sources/widgetLinhaDoTempo.cpp

HEADERS += \
    headers/janelaPrincipal.h \
    headers/leitorcsv.h \
    headers/processo.h \
    headers/escalonador.h \
    headers/gerenciadorMemoria.h \
    headers/simulador.h \
    headers/widgetLinhaDoTempo.h

FORMS += \
    janelaPrincipal.ui

INCLUDEPATH += headers

# Warnings extras para qualidade de código
QMAKE_CXXFLAGS += -Wall -Wextra

# Diretório de saída
DESTDIR  = build
OBJECTS_DIR = build/.obj
MOC_DIR     = build/.moc
UI_DIR      = build/.ui
