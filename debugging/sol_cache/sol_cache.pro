TEMPLATE = app

TARGET = sol_cache

QT += widgets
CONFIG += force_debug_info debug c++11

SOURCES  += sol_cache.cpp \
            cachedsin.cpp \
            plotwidget.cpp

HEADERS += cachedsin.h \
           plotwidget.h

OTHER_FILES += README-GDB.txt

# make M_PI available on Windows
DEFINES += _USE_MATH_DEFINES

