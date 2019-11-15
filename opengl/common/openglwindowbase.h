#ifndef OPENGLWINDOWBASE_H
#define OPENGLWINDOWBASE_H

#include <QtGlobal>
#include <QScopedPointer>
#include <QElapsedTimer>

class AbstractScene;
class CameraController;
class OpenGLDebugLogger;

class QKeyEvent;
class QMouseEvent;
class QOpenGLContext;

/*
 * Mix-in abstract class to allow scenes to be used with both pure GL surfaces
 * (like QWindow) and with QQ2 surfaces (like QQuickWindow).
 *
 * The respective implementations are in OpenGLWindow and QuickOpenGLWindow.
 */

class OpenGLWindowBase
{
    Q_DISABLE_COPY(OpenGLWindowBase)

public:
    OpenGLWindowBase();
    virtual ~OpenGLWindowBase();

    virtual void setScene( AbstractScene *scene );
    AbstractScene *scene() const;

    virtual QOpenGLContext *openglContext() const = 0;

    bool isPaused() const { return m_paused; }

    void togglePause();
protected:
    virtual void initializeGL();
    virtual void resizeGL( int width, int height );
    virtual void paintGL();
    virtual void updateScene();

protected:
    virtual void keyPressEvent( QKeyEvent *e );
    virtual void keyReleaseEvent( QKeyEvent *e );
    virtual void mousePressEvent( QMouseEvent *e );
    virtual void mouseReleaseEvent( QMouseEvent *e );
    virtual void mouseMoveEvent( QMouseEvent *e );

private:
    QScopedPointer<OpenGLDebugLogger> m_logger;
    QScopedPointer<AbstractScene> m_scene;
    CameraController *m_controller;
    QElapsedTimer m_time;
    QElapsedTimer m_frameTime;
    QElapsedTimer m_cameraControllerTime; // Never pauses so we can move camera even when paused

    bool m_paused;
    quint64 m_baseElapsedTime;
};

#endif // OPENGLWINDOWBASE_H
