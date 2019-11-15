#ifndef ABSTRACTSCENE_H
#define ABSTRACTSCENE_H

#include <QObject>
#include <QMutex>

class QOpenGLContext;

class AbstractScene : public QObject
{
    Q_OBJECT

public:
    explicit AbstractScene( QObject* parent = nullptr );
    ~AbstractScene();

    void setContext( QOpenGLContext* context ) { m_context = context; }
    QOpenGLContext* context() const { return m_context; }

    /**
      Load textures, initialize shaders, etc.
      */
    virtual void initialise() = 0;
    /**
      Destroy textures, shaders, etc.; useful if you want to control
      the cleanup (f.i. if it has to happen in another thread)
      */
    virtual void cleanup() {}

    /**
      This is called prior to every frame.  Use this
      to update your animation.
      */
    virtual void update( float t );

    /**
      Draw your scene.
      */
    virtual void render() = 0;

    /**
      Called when screen is resized
      */
    virtual void resize( int w, int h ) = 0;

    void togglePause();
    bool isPaused() const { return m_paused; }

    void adjustModelRotationRate( float offset );

    float modelTheta() const;

    QMutex *mutex() const { return &m_mutex; }

protected:
    QOpenGLContext* m_context;

private:
    mutable QMutex m_mutex; // for protecing the scene from concurrent access
    bool m_paused;
    float m_modelTheta, m_lastTime;
    float m_thetaDegreesPerSecond;
};

#endif // ABSTRACTSCENE_H
