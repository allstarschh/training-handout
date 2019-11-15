TEMPLATE = app

QT += widgets
CONFIG += release force_debug_info

TARGET = lab_mandelbrot

SOURCES  += lab_mandelbrot.cpp \
            mandelbrotwidget.cpp \
            mandelbrot.cpp

HEADERS +=  mandelbrotwidget.h \
            mandelbrot.h

OTHER_FILES += README.perf.txt \
               README.visualstudio.txt

