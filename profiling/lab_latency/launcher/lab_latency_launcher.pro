TEMPLATE = app

QT = core sql

TARGET = lab_latency_launcher

CONFIG += release force_debug_info console

SOURCES  += lab_latency_launcher.cpp \
            setup.cpp \
            launcher.cpp

HEADERS += setup.h \
           launcher.h

CONFIG += c++11

