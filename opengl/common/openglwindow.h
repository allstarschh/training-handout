#ifndef OPENGLWINDOW_H
#define OPENGLWINDOW_H

#include <QWindow>

#include "openglwindowbase.h"

class CameraScene;
class AbstractScene;
class CameraController;
class AxisGnomon;
class QOpenGLDebugMessage;
class OpenGLDebugMessageWindow;

class OpenGLWindow : public QWindow, public OpenGLWindowBase
{
    Q_OBJECT

public:
    explicit OpenGLWindow( const QSurfaceFormat& format,
                           QScreen* parent = nullptr );

    QOpenGLContext *openglContext() const override;

    void setScene( AbstractScene* scene ) override;

    AxisGnomon *axisGnomon() const;
    void setAxisGnomon(AxisGnomon *gnomon);

protected:
    void initializeGL() override;
    void resizeGL( int width, int height ) override;
    void paintGL() override;

    bool event(QEvent *e) override;

    void resizeEvent( QResizeEvent *e ) override;

    void keyPressEvent( QKeyEvent *e ) override;
    void keyReleaseEvent( QKeyEvent *e ) override;
    void mousePressEvent( QMouseEvent *e ) override;
    void mouseReleaseEvent( QMouseEvent *e ) override;
    void mouseMoveEvent( QMouseEvent *e )override;

protected slots:
    void updateScene() override;

private:
    QOpenGLContext* m_context;
    AxisGnomon* m_gnomon;
};

#endif // OPENGLWINDOW_H
