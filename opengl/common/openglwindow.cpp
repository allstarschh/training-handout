#include "openglwindow.h"

#include "axisgnomon.h"
#include "camerascene.h"
#include "events.h"

#include <QGuiApplication>
#include <QSurfaceFormat>
#include <QOpenGLContext>
#include <QResizeEvent>

OpenGLWindow::OpenGLWindow( const QSurfaceFormat &format,
                            QScreen *screen )
    : QWindow( screen )
    , m_context( nullptr )
    , m_gnomon( nullptr )
{
    // Tell Qt we will use OpenGL for this window
    setSurfaceType( OpenGLSurface );

    // Request a full screen button (if available)
#if !defined(Q_OS_WIN)
    setFlags( flags() | Qt::WindowFullscreenButtonHint );
#endif

    QSurfaceFormat actualFormat = format;

    if (qEnvironmentVariableIntValue("OPENGL_DEBUG"))
        actualFormat.setOption(QSurfaceFormat::DebugContext);

    // Create the native window
    setFormat( actualFormat );
    create();

    // Create an OpenGL context
    m_context = new QOpenGLContext;
    m_context->setFormat( actualFormat );
    if (!m_context->create()) {
        qFatal("Context creation failed");
    }

    // Set an initial window size (forces device pixel ratio to be applied
    // within our resizeGL)
    resize( 800, 600 );
}

QOpenGLContext *OpenGLWindow::openglContext() const
{
    return m_context;
}

void OpenGLWindow::setScene( AbstractScene* scene )
{
    OpenGLWindowBase::setScene( scene );

    initializeGL();

    // Do the first update. Subsequent updates will be triggered
    // automatically by this call but allow control to return to
    // the event loop in between frames
    updateScene();
}

AxisGnomon *OpenGLWindow::axisGnomon() const
{
    return m_gnomon;
}

void OpenGLWindow::setAxisGnomon(AxisGnomon *gnomon)
{
    if (m_gnomon == gnomon)
        return;

    if (m_gnomon)
        m_gnomon->deleteLater();

    m_gnomon = gnomon;

    if (m_gnomon) {
        m_gnomon->setParent(this);
        m_gnomon->setWindowSize(size());
    }
}

void OpenGLWindow::initializeGL()
{
    m_context->makeCurrent( this );
    OpenGLWindowBase::initializeGL();
    setAxisGnomon(new AxisGnomon);
}

void OpenGLWindow::resizeGL(int width, int height)
{
    m_context->makeCurrent( this );
    OpenGLWindowBase::resizeGL( width, height );
    if (m_gnomon)
        m_gnomon->setWindowSize(QSize(width, height));
}

void OpenGLWindow::paintGL()
{
    if ( !isExposed() )
        return;

    // Make the context current
    m_context->makeCurrent( this );

    OpenGLWindowBase::paintGL();

    if (m_gnomon) {
        CameraScene* camScene = qobject_cast<CameraScene *>( scene() );
        if (camScene && camScene->camera())
            m_gnomon->render(camScene->camera());
    }

    // Swap front/back buffers
    m_context->swapBuffers( this );
}

bool OpenGLWindow::event(QEvent *e)
{
    if (e->type() == UpdateEvent::eventType()) {
        updateScene();
        return true;
    }
    return QWindow::event(e);
}

void OpenGLWindow::updateScene()
{
    OpenGLWindowBase::updateScene();
    paintGL();

    // Post an event to trigger another update. This relies upon
    // vsync to limit the rate of updates.
    QGuiApplication::postEvent(this, new UpdateEvent);
}

void OpenGLWindow::resizeEvent( QResizeEvent* e )
{
    // HACK: To work around the regression introduced by the fix for QTBUG-67777
    if (m_context)
        resizeGL( e->size().width() * devicePixelRatio(), e->size().height() * devicePixelRatio() );
    QWindow::resizeEvent( e );
}

void OpenGLWindow::keyPressEvent( QKeyEvent* e )
{
    OpenGLWindowBase::keyPressEvent( e );
    if ( !e->isAccepted() )
        QWindow::keyPressEvent( e );
}

void OpenGLWindow::keyReleaseEvent( QKeyEvent* e )
{
    OpenGLWindowBase::keyReleaseEvent( e );
    if ( !e->isAccepted() )
        QWindow::keyReleaseEvent( e );
}

void OpenGLWindow::mousePressEvent( QMouseEvent* e )
{
    OpenGLWindowBase::mousePressEvent( e );
    if ( !e->isAccepted() )
        QWindow::mousePressEvent( e );
}

void OpenGLWindow::mouseReleaseEvent( QMouseEvent* e )
{
    OpenGLWindowBase::mouseReleaseEvent( e );
    if ( !e->isAccepted() )
        QWindow::mouseReleaseEvent( e );
}

void OpenGLWindow::mouseMoveEvent( QMouseEvent* e )
{
    OpenGLWindowBase::mouseMoveEvent( e );
    if ( !e->isAccepted() )
        QWindow::mouseMoveEvent( e );
}
