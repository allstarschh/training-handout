#ifndef MAINMACROS_H
#define MAINMACROS_H

#include <QtGlobal>
#include <QSurfaceFormat>
#include <QDebug>
#include <QString>

#include "resources.h"
#include "assetdownloader.h"

/*
 * This file defines a series of macros to
 *
 * 1) avoid c&p the same identical code in all main.cpp
 *
 * 2) try to support at the same time the combination of
 *    * Desktop GL builds (Linux, Windows, OSX)
 *    * Dynamic GL builds (Windows)
 *    * GL ES builds (Android, iOS)
 *
 * To keep it simple, currently only the following two scenarios are supported:
 *
 * 1) Request a desktop GL (optionally specifying a version), which will fail
 * if Desktop GL is not available and/or the version is not supported
 *
 * 2) Request a "generic" GL, which will return either GL 3.3 Core (on Desktop)
 * or GLES 3.0, or fail
 *
 * In other words, ES2 is never supported, and desktop GL <= 3.3 requires
 * explicit asking. The reasons are:
 *
 * * we have no pure ES2 examples;
 *
 * * we do have some desktop GL only examples (like the geo/tess)
 *
 * * the big majority use the "common subset" of GL3/ES3, like VAOs, modern
 * shaders ("texture" instead of "texture2D", "out" instead of "varying", etc.)
 *
 *
 * === WHICH MACRO TO PICK ===
 *
 * 99% of the time your main will just create a window, create a scene, match them together.
 *
 * If that's the case, and your example should run on both Desktop and GLES,
 * then use SIMPLE_OPENGL_MAIN or SIMPLE_OPENGL_MAIN_WITH_COMMANDLINE.
 *
 * If instead app is desktop only, use DESKTOP_OPENGL_MAIN or
 * DESKTOP_OPENGL_MAIN_WITH_COMMANDLINE.
 *
 * If your app needs to do more stuff in main, you can't use one of those
 * macros, but you should still initialize OpenGL correctly (or nothing will
 * work on Windows/dynGL, you won't have assets, etc.); to do that you can use
 * inside your custom main SIMPLE_OPENGL_QT_APPLICATION or
 * DESKTOP_OPENGL_QT_APPLICATION (the "app" parameter is the name of the
 * Q(Gui)Application instance it creates for you).
 */

// use QApplication or QGuiApplication depending if the app has widgets support
#if defined(QT_WIDGETS_LIB)
#include <QApplication>
typedef QApplication OpenGLApplication;
#else
#include <QGuiApplication>
typedef QGuiApplication OpenGLApplication;
#endif // QT_WIDGETS_LIB

// some helpers for the main function
void initializeDesktopGL( int majorVersion, int minorVersion );
void initializeGenericGL();
void finalizeOpenGLInitialization();

#define INITIALIZE_ASSETS() \
    do { \
        AssetDownloader downloader; \
        if ( !downloader.downloadAssets() ) \
            qFatal( "Unable to download asset files" ); \
        initializeAssetResources(); \
    } while ( 0 )

#if defined(Q_OS_ANDROID)
#define SHOW_WINDOW(w) w.showFullScreen()
#else
#define SHOW_WINDOW(w) w.show()
#endif

// the main function itself

#define OPENGL_QT_APPLICATION( OpenGLInitialization, app ) \
    OpenGLInitialization; \
    OpenGLApplication app( argc, argv ); \
    finalizeOpenGLInitialization(); \
    INITIALIZE_ASSETS()

#define DESKTOP_OPENGL_QT_APPLICATION( majorVersion, minorVersion, app ) \
    OPENGL_QT_APPLICATION( initializeDesktopGL( majorVersion, minorVersion ), app )

#define SIMPLE_OPENGL_QT_APPLICATION( app ) \
    OPENGL_QT_APPLICATION( initializeGenericGL(), app )

#define GENERIC_OPENGL_MAIN( OpenGLInitialization, WindowClass, SceneClass ) \
int main( int argc, char **argv ) \
{ \
    OPENGL_QT_APPLICATION( OpenGLInitialization, app ); \
    app.setWindowIcon(QIcon(":/images/kdab.png")); \
    WindowClass w( QSurfaceFormat::defaultFormat() ); \
    w.setScene( new SceneClass ); \
    SHOW_WINDOW(w); \
    return app.exec(); \
}

#define GENERIC_OPENGL_MAIN_WITH_COMMANDLINE( OpenGLInitialization, WindowClass, SceneClass ) \
int main( int argc, char **argv ) \
{ \
    OPENGL_QT_APPLICATION( OpenGLInitialization, app ); \
    app.setWindowIcon(QIcon(":/images/kdab.png")); \
    const QStringList args = app.arguments(); \
    QString fileName; \
    if ( args.size() != 2 ) { \
        qDebug() << "Please specify an obj file to load, defaulting to ogre head"; \
        fileName = QStringLiteral(":/meshes/ogrehead/ogrehead.obj"); \
    } else { \
        fileName = args.last(); \
    } \
    WindowClass w( QSurfaceFormat::defaultFormat() ); \
    w.setScene( new SceneClass( fileName ) ); \
    SHOW_WINDOW(w); \
    return app.exec(); \
}

#define DESKTOP_OPENGL_MAIN( majorVersion, minorVersion, WindowClass, SceneClass ) \
    GENERIC_OPENGL_MAIN( initializeDesktopGL( majorVersion, minorVersion ), WindowClass, SceneClass )

#define SIMPLE_OPENGL_MAIN( WindowClass, SceneClass ) \
    GENERIC_OPENGL_MAIN( initializeGenericGL(), WindowClass, SceneClass )

#define DESKTOP_OPENGL_MAIN_WITH_COMMANDLINE( majorVersion, minorVersion, WindowClass, SceneClass ) \
    GENERIC_OPENGL_MAIN_WITH_COMMANDLINE( initializeDesktopGL( majorVersion, minorVersion ), WindowClass, SceneClass )

#define SIMPLE_OPENGL_MAIN_WITH_COMMANDLINE( WindowClass, SceneClass ) \
    GENERIC_OPENGL_MAIN_WITH_COMMANDLINE( initializeGenericGL(), WindowClass, SceneClass )

#endif // MAINMACROS_H
