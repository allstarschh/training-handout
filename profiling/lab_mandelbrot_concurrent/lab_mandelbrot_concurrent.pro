TEMPLATE = app

QT += widgets concurrent
CONFIG += release force_debug_info

TARGET = lab_mandelbrot_concurrent

SOURCES  += lab_mandelbrot_concurrent.cpp \
            mandelbrotwidget.cpp \
            mandelbrot.cpp

HEADERS +=  mandelbrotwidget.h \
            mandelbrot.h

OTHER_FILES += README.perf.txt \
               README.visualstudio.txt

CONFIG += c++11

