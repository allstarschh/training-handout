#ifndef QUICKOPENGLWINDOW_H
#define QUICKOPENGLWINDOW_H

#include "openglwindowbase.h"

#include <QQuickView>

class QTimer;

class QuickOpenGLWindow : public QQuickView, public OpenGLWindowBase
{
    Q_OBJECT

public:
    explicit QuickOpenGLWindow( QWindow *parent = nullptr );
    explicit QuickOpenGLWindow( const QSurfaceFormat &format, QWindow *parent = nullptr );

    void setScene( AbstractScene *scene ) override;

    QOpenGLContext *openglContext() const override;

private slots:
    void initializeOpenGLScene();
    void renderOpenGLScene();
    void cleanupOpenGLScene();
    void update();

    void onSceneGraphError(QQuickWindow::SceneGraphError error, const QString & message);

protected:
    bool event(QEvent *e) override;

    void resizeEvent( QResizeEvent *e ) override;

    void keyPressEvent( QKeyEvent *e ) override;
    void keyReleaseEvent( QKeyEvent *e ) override;
    void mousePressEvent( QMouseEvent *e ) override;
    void mouseReleaseEvent( QMouseEvent *e ) override;
    void mouseMoveEvent( QMouseEvent *e ) override;

private:
    void initialize( const QSurfaceFormat &format );
};

#endif // QUICKOPENGLWINDOW_H
