#include "openglwindowbase.h"

#include "camerascene.h"
#include "cameracontroller.h"

#include "opengldebuglogger.h"

#include <QGuiApplication>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

#ifndef GL_MULTISAMPLE // OpenGL ES doesn't define this
#define GL_MULTISAMPLE 0x809D
#endif

OpenGLWindowBase::OpenGLWindowBase( )
    : m_logger( new OpenGLDebugLogger )
    , m_scene( nullptr )
    , m_controller( new CameraController )
    , m_paused( false )
    , m_baseElapsedTime( 0 )
{
}

OpenGLWindowBase::~OpenGLWindowBase()
{
}

void OpenGLWindowBase::setScene( AbstractScene *scene )
{
    // We take ownership of the scene
    Q_ASSERT( scene );
    Q_ASSERT( !m_scene );
    m_scene.reset( scene );
    m_scene->setParent( nullptr );

    CameraScene* camScene = qobject_cast<CameraScene *>( scene );
    if ( camScene ) {
        m_controller->setCamera( camScene->camera() );
        camScene->setCameraController( m_controller );
    }
}

AbstractScene *OpenGLWindowBase::scene() const
{
    return m_scene.data();
}

void OpenGLWindowBase::togglePause()
{
    m_paused = !m_paused;
    if ( m_paused ) {
        // just become paused
        m_baseElapsedTime += m_time.elapsed();
    } else {
        // unpause
        m_time.restart();
        m_frameTime.restart();
    }
}

void OpenGLWindowBase::initializeGL()
{
    QSurfaceFormat contextFmt = QOpenGLContext::currentContext()->format();
    if (contextFmt.majorVersion() < 3) {
        qFatal("Context version is: %d.%d, needs to be 3",
               contextFmt.majorVersion(), contextFmt.minorVersion());
    }

    m_logger->initialize();

    {
        QMutexLocker locker(m_scene->mutex());
        m_scene->setContext( openglContext() );
        m_scene->initialise();
    }

    m_time.start();
    m_frameTime.start();
    m_cameraControllerTime.start();
}

void OpenGLWindowBase::resizeGL( int width, int height )
{
    if (!QOpenGLContext::currentContext())
        return;

    QMutexLocker locker(m_scene->mutex());

    m_scene->resize( width, height );
}

void OpenGLWindowBase::paintGL()
{
    if ( !QOpenGLContext::currentContext()->isOpenGLES() ) {
        QOpenGLFunctions *functions = QOpenGLContext::currentContext()->functions();
        if ( m_controller->isMultisampleEnabled() )
            functions->glEnable( GL_MULTISAMPLE );
        else
            functions->glDisable( GL_MULTISAMPLE );
    }

    // Do the rendering (to the back buffer)
    m_scene->render();
}

void OpenGLWindowBase::updateScene()
{
    float time = (m_time.elapsed() + m_baseElapsedTime) / 1000.0f;
    if ( m_paused )
        time = m_baseElapsedTime / 1000.0f;

    {
        QMutexLocker locker(m_scene->mutex());
        m_controller->update( m_cameraControllerTime.elapsed() / 1000.0f );
        m_scene->update( time );
    }
}

void OpenGLWindowBase::keyPressEvent( QKeyEvent *e )
{
    QMutexLocker locker(m_scene->mutex());
    if ( m_controller->keyPressEvent( e ) )
      return;

    switch ( e->key() )
    {
        case Qt::Key_Escape:
            QGuiApplication::quit();
            e->accept();
            break;
        case Qt::Key_F5:
            m_logger->showDebugWindow();
            e->accept();
            break;

        case Qt::Key_Less:
            m_scene->adjustModelRotationRate( -10.0f );
            e->accept();
            break;

        case Qt::Key_Greater:
            m_scene->adjustModelRotationRate( 10.0f );
            e->accept();
            break;

       case Qt::Key_Space:
            m_scene->togglePause();
            togglePause();
            e->accept();
            break;
    }
}

void OpenGLWindowBase::keyReleaseEvent( QKeyEvent *e )
{
    QMutexLocker locker(m_scene->mutex());
    m_controller->keyReleaseEvent( e );
}

void OpenGLWindowBase::mousePressEvent( QMouseEvent *e )
{
    QMutexLocker locker(m_scene->mutex());
    m_controller->mousePressEvent( e );
}

void OpenGLWindowBase::mouseReleaseEvent( QMouseEvent *e )
{
    QMutexLocker locker(m_scene->mutex());
    m_controller->mouseReleaseEvent( e );
}

void OpenGLWindowBase::mouseMoveEvent( QMouseEvent *e )
{
    QMutexLocker locker(m_scene->mutex());
    m_controller->mouseMoveEvent( e );
}
