#ifndef INSTANCEDHISTOGRAMSCENE_H
#define INSTANCEDHISTOGRAMSCENE_H

#include "camerascene.h"

#include <QOpenGLBuffer>
#include <QMatrix4x4>

class Camera;
class Cube;
class Sphere;

class QOpenGLFunctions_3_3_Core;

class InstancedHistogramScene : public CameraScene
{
    Q_OBJECT

public:
    explicit InstancedHistogramScene( QObject* parent = nullptr );

    void initialise() override;
    void update( float t ) override;
    void render() override;

    void toggleUpdates()
    {
        m_updatesEnabled = !m_updatesEnabled;
    }

private:
    void prepareVertexBuffers();
    void prepareVertexArrayObject();
    void prepareTexture();

    QOpenGLFunctions_3_3_Core* m_funcs;

    Cube* m_cube;

    float m_theta;
    QMatrix4x4 m_modelMatrix;

    // The data arrays and corresponding buffers
    QVector<float> m_data;
    QOpenGLBuffer m_dataBuffer;

    // Sphere for the backgroun
    Sphere* m_sphere;
    QMatrix4x4 m_sphereModelMatrix;
    float m_time;

    bool m_updatesEnabled;
};

#endif // INSTANCEDHISTOGRAMSCENE_H
