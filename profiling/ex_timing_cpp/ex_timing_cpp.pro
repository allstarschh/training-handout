TEMPLATE = app

DEFINES += "INPUT_FILE=\\\"$$_PRO_FILE_PWD_/../data/qt-devel-june-2015.txt\\\""

QT -= core
CONFIG += release force_debug_info c++11 console
SOURCES += ex_timing_cpp.cpp
HEADERS += timetrace.h
RESOURCES += resources.qrc

