#include "basicwindow.h"

#include "abstractscene.h"
#include "events.h"

#include <QGuiApplication>
#include <QKeyEvent>
#include <QOpenGLContext>
#include <QDebug>

BasicOpenGLWindow::BasicOpenGLWindow( const QSurfaceFormat& format,
                            QScreen* screen )
    : QWindow( screen ),
      m_context( nullptr ),
      m_scene( nullptr )
{
    // Tell Qt we will use OpenGL for this window
    setSurfaceType( OpenGLSurface );

    // Request a full screen button (if available)
#if !defined(Q_OS_WIN)
    setFlags( flags() | Qt::WindowFullscreenButtonHint );
#endif

    // Set an initial window size (cocoa qpa needs this)
    resize( 800, 600 );

    // Create the native window
    setFormat( format );
    create();

    // Create an OpenGL context
    m_context = new QOpenGLContext(this);
    m_context->setFormat( format );
    if (!m_context->create()) {
        qFatal("Context creation failed");
    }

    if (m_context->format().majorVersion() < 3) {
        qFatal("Context version is: %d.%d, needs to be 3", m_context->format().majorVersion(),
               m_context->format().minorVersion());
    }
}

void BasicOpenGLWindow::setScene( AbstractScene* scene )
{
    // We take ownership of the scene
    Q_ASSERT( scene );
    m_scene = scene;
    m_scene->setParent( this );

    // Initialise the scene
    m_scene->setContext( m_context );
    initializeGL();

    // Set initial window size
    resize( 800, 600 );

    // Do the first update. Subsequent updates will be triggered
    // automatically by this call but allow control to return to
    // the event loop in between frames
    updateScene();
}

void BasicOpenGLWindow::initializeGL()
{
    m_context->makeCurrent( this );
    m_scene->initialise();

    m_time.start();
}

void BasicOpenGLWindow::resizeGL()
{
    m_context->makeCurrent( this );
    m_scene->resize( width() * devicePixelRatio(), height() * devicePixelRatio() );
}

void BasicOpenGLWindow::paintGL()
{
    if ( !isExposed() )
        return;

    // Make the context current
    m_context->makeCurrent( this );

    // Do the rendering (to the back buffer)
    m_scene->render();

    // Swap front/back buffers
    m_context->swapBuffers( this );
}

bool BasicOpenGLWindow::event(QEvent *e)
{
    if (e->type() == UpdateEvent::eventType()) {
        updateScene();
        return true;
    }
    return QWindow::event(e);
}

void BasicOpenGLWindow::updateScene()
{
    float time = m_time.elapsed() / 1000.0f;
    m_scene->update( time );
    paintGL();

    // Post an event to trigger another update. This relies upon
    // vsync to limit the rate of updates.
    QGuiApplication::postEvent(this, new UpdateEvent);
}

void BasicOpenGLWindow::resizeEvent( QResizeEvent* e )
{
    Q_UNUSED( e );
    if ( m_context )
        resizeGL();
}

void BasicOpenGLWindow::keyPressEvent( QKeyEvent* e )
{
    switch ( e->key() )
    {
        case Qt::Key_Escape:
            QGuiApplication::quit();
            break;

        default:
            QWindow::keyPressEvent( e );
    }
}
