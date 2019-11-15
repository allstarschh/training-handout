#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

class Camera;
class QMouseEvent;
class QWheelEvent;
class QKeyEvent;

#include <QObject>
#include <QPoint>
#include <QVector3D>

class CameraController : public QObject
{
    Q_OBJECT
    Q_PROPERTY( float linearSpeed READ linearSpeed WRITE setLinearSpeed NOTIFY linearSpeedChanged )

    Q_PROPERTY( float orbitRate READ orbitRate WRITE setOrbitRate NOTIFY orbitRateChanged )
    Q_PROPERTY( float lookRate READ lookRate WRITE setLookRate NOTIFY lookRateChanged )

    Q_PROPERTY( bool multisampleEnabled READ isMultisampleEnabled() NOTIFY multisampleEnabledChanged )

    Q_PROPERTY( ControlMode controlMode READ controlMode WRITE setControlMode NOTIFY controlModeChanged )
    Q_PROPERTY( QVector3D firstPersonUpVector READ firstPersonUpVector WRITE setFirstPersonUpVector NOTIFY firstPersonUpVectorChanged )

public:
    explicit CameraController( QObject *parent = nullptr );

    void setCamera( Camera* cam );
    void setLinearSpeed( float speed );
    float linearSpeed() const;

    float orbitRate() const;
    void setOrbitRate( float rate );

    float lookRate() const;
    void setLookRate( float rate );

    void mousePressEvent( QMouseEvent* aEvent );
    void mouseReleaseEvent( QMouseEvent* aEvent );
    void mouseMoveEvent( QMouseEvent* aEvent );

    bool keyPressEvent( QKeyEvent* aEvent );
    bool keyReleaseEvent( QKeyEvent* aEvent );

    bool handleScroll( QWheelEvent* aWheel );

    void update( double t );

    bool isMultisampleEnabled() const;

    enum ControlMode {
        FreeLook,
        FirstPerson
    };
    Q_ENUM( ControlMode )

    void setControlMode( ControlMode controlMode );
    ControlMode controlMode() const;

    void setFirstPersonUpVector( const QVector3D &up );
    QVector3D firstPersonUpVector() const;

public slots:
    void toggleMSAA();

signals:
    void linearSpeedChanged();
    void orbitRateChanged();
    void lookRateChanged();

    void multisampleEnabledChanged();

    void controlModeChanged();
    void firstPersonUpVectorChanged();

private:
    Camera* m_camera;
    float m_vx;
    float m_vy;
    float m_vz;
    bool m_viewCenterFixed;
    float m_panAngle;
    float m_tiltAngle;

    bool m_leftButtonPressed;
    QPoint m_prevPos;
    QPoint m_pos;
    bool m_orbitMode;

    float m_linearSpeed;
    float m_time;
    float m_orbitRate;
    float m_lookRate;

    bool m_multisampleEnabled;

    ControlMode m_controlMode;
    QVector3D m_firstPersonUp;
};

#endif // of CAMERA_CONTROLLER_H
