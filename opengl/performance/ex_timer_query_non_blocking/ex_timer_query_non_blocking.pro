include( ../../common/common.pri )

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            timerquerynonblockingscene.cpp \
            openglframetimer.cpp

HEADERS  += timerquerynonblockingscene.h \
            openglframetimer.h

RESOURCES += ex_timer_query_non_blocking.qrc

OTHER_FILES += shaders/normalmap.vert \
               shaders/normalmap.frag

