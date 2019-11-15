include( embedded_build.pri )
include( opengldebug.pri )
include( common_common.pri )

TEMPLATE = lib
CONFIG += staticlib
TARGET = opengl-common


HEADERS += \
           $$PWD/axisalignedboundingbox.h \
           $$PWD/abstractscene.h \
           $$PWD/camera.h \
           $$PWD/camera_p.h \
           $$PWD/cube.h \
           $$PWD/cone.h \
           $$PWD/fullscreenquad.h \
           $$PWD/material.h \
           $$PWD/mesh.h \
           $$PWD/objloader.h \
           $$PWD/plane.h \
           $$PWD/sampler.h \
           $$PWD/skybox.h \
           $$PWD/torus.h \
           $$PWD/sphere.h \
           $$PWD/grid.h \
           $$PWD/cameracontroller.h \
           $$PWD/basicwindow.h \
           $$PWD/camerascene.h \
           $$PWD/openglwindow.h \
           $$PWD/resources.h \
           $$PWD/logicalplane.h \
           $$PWD/framebufferobject.h \
           $$PWD/openglwindowbase.h \
           $$PWD/axisgnomon.h \
           $$PWD/mainmacros.h \
           $$PWD/glslversionhandler.h \
           $$PWD/opengldebuglogger.h \
           $$PWD/openglshaderprogram.h \
           $$PWD/mathutils.h \
           $$PWD/assetdownloader.h \
           $$PWD/events.h \
           $$PWD/hdrimage.h

SOURCES += \
           $$PWD/axisalignedboundingbox.cpp \
           $$PWD/abstractscene.cpp \
           $$PWD/camera.cpp \
           $$PWD/cube.cpp \
           $$PWD/cone.cpp \
           $$PWD/fullscreenquad.cpp \
           $$PWD/material.cpp \
           $$PWD/mesh.cpp \
           $$PWD/objloader.cpp \
           $$PWD/plane.cpp \
           $$PWD/sampler.cpp \
           $$PWD/skybox.cpp \
           $$PWD/torus.cpp \
           $$PWD/sphere.cpp \
           $$PWD/grid.cpp \
           $$PWD/cameracontroller.cpp \
           $$PWD/basicwindow.cpp \
           $$PWD/camerascene.cpp \
           $$PWD/openglwindow.cpp \
           $$PWD/logicalplane.cpp \
           $$PWD/framebufferobject.cpp \
           $$PWD/openglwindowbase.cpp \
           $$PWD/mainmacros.cpp \
           $$PWD/glslversionhandler.cpp \
           $$PWD/axisgnomon.cpp \
           $$PWD/opengldebuglogger.cpp \
           $$PWD/openglshaderprogram.cpp \
           $$PWD/assetdownloader.cpp \
           $$PWD/events.cpp \
           $$PWD/hdrimage.cpp

!contains(QT_CONFIG, opengles2) {
    SOURCES += \
               $$PWD/kdgeometryshader.cpp

    HEADERS += \
               $$PWD/kdgeometryshader.h

    !macx {
        HEADERS += \
                   $$PWD/indirectcommands.h \
                   $$PWD/meshcollection.h

        SOURCES +=  \
                   $$PWD/meshcollection.cpp
    }
}

have_opengl_debug_window {
    DEFINES += HAVE_OPENGL_DEBUG_WINDOW

    SOURCES += \
               $$PWD/opengldebugmessagemodel.cpp \
               $$PWD/opengldebugmessagewindow.cpp \

    HEADERS += \
               $$PWD/opengldebugmessagemodel.h \
               $$PWD/opengldebugmessagewindow.h
}

qtHaveModule(quick) {
    QT += quick
    HEADERS += $$PWD/quickopenglwindow.h
    SOURCES += $$PWD/quickopenglwindow.cpp
}

#
# Generate the RCC (binary resource) file for all of our assets.
# We can't use RESOURCES here, as the resources are not in a subdir,
# of each example
#
RCC_BINARY_SOURCES += ../assets/opengl-assets.qrc
asset_builder.commands = $$[QT_HOST_BINS]/rcc -binary ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
asset_builder.depend_command = $$[QT_HOST_BINS]/rcc -list $$QMAKE_RESOURCE_FLAGS ${QMAKE_FILE_IN}
asset_builder.input = RCC_BINARY_SOURCES
asset_builder.output = $$OUT_PWD/../assets/${QMAKE_FILE_IN_BASE}.qrb
asset_builder.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += asset_builder

OTHER_FILES += \
    $$PWD/AndroidManifest.xml.in \
    $$PWD/../assets/opengl-assets.qrc \
    $$PWD/../assets/meshes/* \
    $$PWD/../assets/meshes/chest/* \
    $$PWD/../assets/meshes/ogrehead/* \
    $$PWD/../assets/textures/* \
    $$PWD/../assets/textures/cubemap_miramar/* \
    $$PWD/../assets/textures/cubemap_night/* \
    $$PWD/../assets/textures/earth/* \
    $$PWD/../assets/textures/pattern_06/* \
    $$PWD/../assets/textures/pattern_09/* \
    $$PWD/../assets/textures/pattern_10/* \
    $$PWD/../assets/textures/pattern_124/* \
    $$PWD/../assets/textures/pattern_125/* \
    $$PWD/../assets/textures/pattern_128/* \
    $$PWD/../assets/textures/pattern_133/* \
    $$PWD/../assets/textures/terrain/* \
    $$PWD/../assets/shaders/* \
    $$PWD/../assets/images/*

exists($$PWD/../assets/extra/extra.pri) {
    include($$PWD/../assets/extra/extra.pri)
}

