#include "quickopenglwindow.h"

#include "abstractscene.h"
#include "events.h"

#include <QGuiApplication>
#include <QSurfaceFormat>

QuickOpenGLWindow::QuickOpenGLWindow( QWindow *parent )
    : QQuickView( parent )
{
    QSurfaceFormat format;
    format.setProfile( QSurfaceFormat::CoreProfile );
    format.setMajorVersion( 3 );
    format.setMinorVersion( 3 );
    format.setDepthBufferSize( 24 );
    format.setStencilBufferSize( 8 );
    format.setSamples( 8 );
    format.setSwapInterval( 1 );

    initialize( format );
}

QuickOpenGLWindow::QuickOpenGLWindow( const QSurfaceFormat &format,
                                      QWindow *parent )
    : QQuickView( parent )
{
    initialize( format );
}

void QuickOpenGLWindow::initialize( const QSurfaceFormat &format )
{
    QSurfaceFormat actualFormat = format;

    if (qEnvironmentVariableIntValue("OPENGL_DEBUG"))
        actualFormat.setOption(QSurfaceFormat::DebugContext);

    // Create the native window
    setFormat( actualFormat );

    create();

    setClearBeforeRendering( false );

    QObject::connect( this, &QuickOpenGLWindow::sceneGraphInitialized, this, &QuickOpenGLWindow::initializeOpenGLScene, Qt::DirectConnection );
    QObject::connect( this, &QuickOpenGLWindow::beforeRendering, this, &QuickOpenGLWindow::renderOpenGLScene, Qt::DirectConnection );
    QObject::connect( this, &QuickOpenGLWindow::sceneGraphInvalidated, this, &QuickOpenGLWindow::cleanupOpenGLScene, Qt::DirectConnection );
    QObject::connect( this, &QuickOpenGLWindow::sceneGraphError, this, &QuickOpenGLWindow::onSceneGraphError);

    // starting updates here is too early (this might fire before scene initialization)
    // starting in initializeOpenGLScene is wrong (wrong thread)
    // so: start via a posted event posting invoked when initializeOpenGLScene is done
}

void QuickOpenGLWindow::setScene(AbstractScene *scene)
{
    OpenGLWindowBase::setScene( scene );
}

QOpenGLContext *QuickOpenGLWindow::openglContext() const
{
    return QQuickView::openglContext();
}

void QuickOpenGLWindow::initializeOpenGLScene()
{
    OpenGLWindowBase::initializeGL();

    // Schedule the first update - will happen on the main thread
    QGuiApplication::postEvent(this, new UpdateEvent);
}

void QuickOpenGLWindow::renderOpenGLScene()
{
    OpenGLWindowBase::paintGL();

    // Post another update request from here so that we are bound by the
    // vsync rate
    QGuiApplication::postEvent(this, new UpdateEvent);
}

void QuickOpenGLWindow::cleanupOpenGLScene()
{
    scene()->cleanup();
}

void QuickOpenGLWindow::update()
{
    OpenGLWindowBase::updateScene();

    // Ask QQuickView to render the scene
    QQuickView::update();

    // We don't trigger another update here as it won't block on the vsync
    // since all we do here is request an update from QQ2. We need to post
    // a new UpdateEvent from the render thread such that we only post one
    // request per frame. So, let's do it at the end of the render function
}

void QuickOpenGLWindow::onSceneGraphError(QQuickWindow::SceneGraphError error, const QString &message)
{
    qFatal("Scene graph error: %d, %s", error, message.toLatin1().constData());
}

bool QuickOpenGLWindow::event(QEvent *e)
{
    if (e->type() == UpdateEvent::eventType()) {
        update();
        return true;
    }
    return QQuickView::event(e);
}

void QuickOpenGLWindow::resizeEvent( QResizeEvent* e )
{
    resizeGL( e->size().width() * devicePixelRatio(), e->size().height() * devicePixelRatio() );
    QQuickView::resizeEvent( e );
}

// pass input events to QML first. if not handled, propagate them up

void QuickOpenGLWindow::keyPressEvent( QKeyEvent* e )
{
    QQuickView::keyPressEvent( e );
    if ( !e->isAccepted() )
        OpenGLWindowBase::keyPressEvent( e );
}

void QuickOpenGLWindow::keyReleaseEvent( QKeyEvent* e )
{
    QQuickView::keyReleaseEvent( e );
    if ( !e->isAccepted() )
        OpenGLWindowBase::keyReleaseEvent( e );
}

void QuickOpenGLWindow::mousePressEvent( QMouseEvent* e )
{
    QQuickView::mousePressEvent( e );
    if ( !e->isAccepted() )
        OpenGLWindowBase::mousePressEvent( e );
}

void QuickOpenGLWindow::mouseReleaseEvent( QMouseEvent* e )
{
    QQuickView::mouseReleaseEvent( e );
    if ( !e->isAccepted() )
        OpenGLWindowBase::mouseReleaseEvent( e );
}

void QuickOpenGLWindow::mouseMoveEvent( QMouseEvent* e )
{
    QQuickView::mouseMoveEvent( e );
    if ( !e->isAccepted() )
        OpenGLWindowBase::mouseMoveEvent( e );
}
