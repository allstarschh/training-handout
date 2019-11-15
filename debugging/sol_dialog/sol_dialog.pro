TEMPLATE = app

TARGET = sol_dialog

QT += widgets
CONFIG += force_debug_info debug c++11

SOURCES  += sol_dialog.cpp \
            window.cpp \
            dateinputdialog.cpp

HEADERS += window.h \
           dateinputdialog.h

FORMS += window.ui \
         dateinputdialog.ui

