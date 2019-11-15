TEMPLATE = app

QT += widgets concurrent
CONFIG += release force_debug_info

TARGET = sol_mandelbrot_concurrent

SOURCES  += sol_mandelbrot_concurrent.cpp \
            mandelbrotwidget.cpp \
            mandelbrot.cpp

HEADERS +=  mandelbrotwidget.h \
            mandelbrot.h

OTHER_FILES += README.txt

CONFIG += c++11

