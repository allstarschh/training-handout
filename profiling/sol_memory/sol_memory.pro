TEMPLATE = app

QT = core

TARGET = sol_memory

CONFIG += debug force_debug_info console

SOURCES  += sol_memory.cpp \
            worker.cpp \
            mailparser.cpp \

HEADERS += worker.h \
           mailparser.h \

DEFINES += "INPUT_FILE=\\\"$$_PRO_FILE_PWD_/../../data/qt-devel-june-2015.txt\\\""

OTHER_FILES += README.heaptrack.txt

CONFIG += c++11

