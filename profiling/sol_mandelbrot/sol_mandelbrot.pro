TEMPLATE = app

QT += widgets
CONFIG += release force_debug_info

TARGET = sol_mandelbrot

SOURCES  += sol_mandelbrot.cpp \
            mandelbrotwidget.cpp \
            mandelbrot.cpp

HEADERS +=  mandelbrotwidget.h \
            mandelbrot.h

OTHER_FILES += README.txt

