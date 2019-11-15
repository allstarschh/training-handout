#include "mainmacros.h"

#include <QtGlobal>
#include <QDebug>
#include <QOpenGLContext>

static enum {
    DesktopGL,
    GenericGL
} s_openGLImplementation;

static void setDefaultOptionsForFormat( QSurfaceFormat *format )
{
    format->setStencilBufferSize( 8 );
    format->setDepthBufferSize( 24 );
    format->setSamples( 4 );
    format->setSwapInterval( 1 );
}

void initializeDesktopGL( int majorVersion, int minorVersion )
{
#ifndef QT_OPENGL_ES
    QSurfaceFormat format;
    format.setRenderableType( QSurfaceFormat::OpenGL );
    format.setVersion( majorVersion, minorVersion );
    format.setProfile( QSurfaceFormat::CoreProfile );
    setDefaultOptionsForFormat( &format );
    QSurfaceFormat::setDefaultFormat( format );

    QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL );

    s_openGLImplementation = DesktopGL;

    // TODO: do a sanity check that indeed everything is OK
#else
    qFatal("Desktop GL is not supported on this platform");
#endif
}

void initializeGenericGL()
{
    // don't do anything right here. We need to create a QGuiApplication first,
    // then we'll finalize the surface format etc. after
    s_openGLImplementation = GenericGL;
}

void finalizeOpenGLInitialization()
{
    switch ( s_openGLImplementation ) {
    case DesktopGL:
        // There's actually nothing to do; just check that we can indeed create a context of the specified format (later)
        break;

    case GenericGL: {
        // Create a context with a dummy format. That will tell us if we're Desktop GL or ES
        QOpenGLContext dummyContext;
        if ( !dummyContext.create() )
            qFatal("Could not create a temporary GL context for checking GL capabilities");

        // Now build the default surface format
        QSurfaceFormat format;
        setDefaultOptionsForFormat( &format );

        if ( dummyContext.isOpenGLES() || qEnvironmentVariableIsSet( "FORCE_OPENGL_ES" ) ) {
             // EXT_create_context_es2_profile: create an ES3 context under an OpenGL desktop implementation
            format.setRenderableType( QSurfaceFormat::OpenGLES );
            format.setVersion( 3, 1 );
        } else {
            format.setRenderableType( QSurfaceFormat::OpenGL );
            format.setVersion( 3, 3 );
            format.setProfile( QSurfaceFormat::CoreProfile );
        }

        QSurfaceFormat::setDefaultFormat( format );
    }
        break;
    }

    // Sanity check
    QOpenGLContext context;
    context.setFormat( QSurfaceFormat::defaultFormat() );

    if ( !context.create() ) {
        qCritical() << "Could not create a GL context with the specified format" << QSurfaceFormat::defaultFormat();
        qFatal("Sanity checks on the GL version failed.");
    }
}
