TEMPLATE = app

CONFIG -= qt
CONFIG += debug console
SOURCES  += ex_random_behavior.cpp

OTHER_FILES += \
    commands.txt

unix:LIBS += -lpthread

