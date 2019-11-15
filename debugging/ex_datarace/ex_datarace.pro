TEMPLATE = app

CONFIG -= qt
CONFIG += force_debug_info c++11 debug console
SOURCES += ex_datarace.cpp

unix:LIBS += -lpthread

