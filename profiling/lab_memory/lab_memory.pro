TEMPLATE = app

QT = core

TARGET = lab_memory

# In this case we disable optimizations to avoid any inlining (for better visualisation in heaptrack).
# The use of malloc and free is the same with and without optimizations.
CONFIG += debug force_debug_info console

SOURCES  += lab_memory.cpp \
            worker.cpp \
            mailparser.cpp \

HEADERS += worker.h \
           mailparser.h \

DEFINES += "INPUT_FILE=\\\"$$_PRO_FILE_PWD_/../data/qt-devel-june-2015.txt\\\""

OTHER_FILES += README.heaptrack.txt \
               README.visualstudio.txt

CONFIG += c++11

