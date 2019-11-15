#ifndef BASICTIMERQUERYSCENE_H
#define BASICTIMERQUERYSCENE_H

#include <camerascene.h>
#include <material.h>

#include <QMatrix4x4>
#include <QStringList>

class Camera;
class Mesh;
class QOpenGLFunctions;

class OpenGLFrameTimer;

class TimerQueryNonBlockingScene : public CameraScene
{
public:
    TimerQueryNonBlockingScene( QObject* parent = nullptr );

    void initialise() override;
    void update( float t ) override;
    void render() override;

private:
    MaterialPtr createTextureMaterial( const QString& diffuse,
                                       const QString& normal );

    // Mesh-related members
    QMatrix4x4 m_modelMatrix;
    Mesh* m_mesh;

    QStringList m_renderStages;
    OpenGLFrameTimer *m_frameTimer;

    QOpenGLFunctions* m_funcs;
};

#endif // BASICTIMERQUERYSCENE_H
