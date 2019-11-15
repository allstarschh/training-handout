#ifndef BASICWINDOW_H
#define BASICWINDOW_H

#include <QWindow>
#include <QTime>

class AbstractScene;

class BasicOpenGLWindow : public QWindow
{
    Q_OBJECT

public:
    explicit BasicOpenGLWindow( const QSurfaceFormat& format,
                                QScreen* parent = nullptr );

    QOpenGLContext* context() const { return m_context; }

    void setScene( AbstractScene* scene );
    AbstractScene* scene() const { return m_scene; }
    
protected:
    virtual void initializeGL();
    virtual void resizeGL();
    virtual void paintGL();

protected slots:
    virtual void updateScene();

protected:
    bool event(QEvent *e) override;
    void resizeEvent( QResizeEvent* e ) override;
    void keyPressEvent( QKeyEvent* e ) override;

private:
    QOpenGLContext* m_context;
    AbstractScene* m_scene;
    
    QTime m_time;
};

#endif // BASICWINDOW_H
