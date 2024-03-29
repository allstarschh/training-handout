include( ../../common/common.pri )

QT += opengl

TEMPLATE = app

INCLUDEPATH += ../../common

SOURCES  += main.cpp \
            instancedhistogramscene.cpp \

HEADERS  += \
            instancedhistogramscene.h \

RESOURCES += ex_instanced_histogram.qrc

OTHER_FILES += shaders/instancedhistogram.vert \
               shaders/instancedhistogram.frag \
               shaders/noise.vert \
               shaders/noise.frag

