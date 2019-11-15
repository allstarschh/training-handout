#ifndef CAMERASCENE_H
#define CAMERASCENE_H

#include "abstractscene.h"

#include "camera.h"

class CameraController;

class CameraScene : public AbstractScene
{
    Q_OBJECT

public:
    explicit CameraScene( QObject* parent = nullptr );

    void resize( int w, int h ) override;

    Camera* camera() const { return m_camera; }
    CameraController* cameraController() const { return m_controller; }

protected:
    friend class OpenGLWindowBase;
    void setCameraController( CameraController* controller ) { m_controller = controller; }

    Camera* m_camera;
    CameraController* m_controller;
};

#endif // CAMERASCENE_H
