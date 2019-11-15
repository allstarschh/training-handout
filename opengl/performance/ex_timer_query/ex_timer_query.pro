include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            timerqueryscene.cpp

HEADERS  += timerqueryscene.h

RESOURCES += ex_timer_query.qrc

OTHER_FILES += shaders/normalmap.vert \
               shaders/normalmap.frag

