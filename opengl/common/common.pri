include( ../assets/assets.pri )
include( embedded_build.pri )
include( opengldebug.pri )
include( common_common.pri )

win32 {
    build_pass {
        CONFIG(debug, debug|release) {
            common_static_lib = $$OUT_PWD/../../common/debug/$${QMAKE_PREFIX_STATICLIB}opengl-common.$${QMAKE_EXTENSION_STATICLIB}
        } else {
            common_static_lib = $$OUT_PWD/../../common/release/$${QMAKE_PREFIX_STATICLIB}opengl-common.$${QMAKE_EXTENSION_STATICLIB}
        }
    }
} else {
    common_static_lib = $$OUT_PWD/../../common/$${QMAKE_PREFIX_STATICLIB}opengl-common.$${QMAKE_EXTENSION_STATICLIB}
}

LIBS += $$common_static_lib
PRE_TARGETDEPS += $$common_static_lib

android {
    # automatically generate the manifest for Android
    ANDROID_PACKAGE_SOURCE_DIR = android

    android_manifest.input = $$PWD/AndroidManifest.xml.in
    android_manifest.output = $$ANDROID_PACKAGE_SOURCE_DIR/AndroidManifest.xml

    QMAKE_SUBSTITUTES += android_manifest
}

windows {
    RC_ICONS =  $$PWD/../assets/images/kdab.ico
}

mac {
    ICON = $$PWD/../assets/images/kdab.icns
}
