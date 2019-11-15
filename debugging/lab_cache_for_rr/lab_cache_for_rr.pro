TEMPLATE = app

TARGET = lab_cache_for_rr

QT += widgets
CONFIG += force_debug_info c++11

SOURCES  += lab_cache_for_rr.cpp \
            cachedsin.cpp \
            plotwidget.cpp

HEADERS += cachedsin.h \
           plotwidget.h

# make M_PI available on Windows
DEFINES += _USE_MATH_DEFINES

