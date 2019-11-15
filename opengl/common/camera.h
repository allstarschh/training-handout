#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>

#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector3D>
#include <QSharedPointer>

#include "logicalplane.h"

class CameraPrivate;

class QOpenGLShaderProgram;
typedef QSharedPointer<QOpenGLShaderProgram> QOpenGLShaderProgramPtr;

class Camera : public QObject
{
    Q_OBJECT

    Q_PROPERTY( QVector3D position READ position WRITE setPosition )
    Q_PROPERTY( QVector3D upVector READ upVector WRITE setUpVector )
    Q_PROPERTY( QVector3D viewCenter READ viewCenter WRITE setViewCenter )

    Q_PROPERTY( ProjectionType projectionType READ projectionType )
    Q_PROPERTY( float nearPlane READ nearPlane WRITE setNearPlane )
    Q_PROPERTY( float farPlane READ farPlane WRITE setFarPlane )

    Q_PROPERTY( float fieldOfView READ fieldOfView WRITE setFieldOfView )
    Q_PROPERTY( float aspectRatio READ aspectRatio WRITE setAspectRatio )

    Q_PROPERTY( float left READ left WRITE setLeft )
    Q_PROPERTY( float right READ right WRITE setRight )
    Q_PROPERTY( float bottom READ bottom WRITE setBottom )
    Q_PROPERTY( float top READ top WRITE setTop )


public:
    explicit Camera( QObject* parent = nullptr );

    enum ProjectionType
    {
        OrthogonalProjection,
        PerspectiveProjection
    };
    Q_ENUM( ProjectionType )

    enum CameraTranslationOption
    {
        TranslateViewCenter,
        DontTranslateViewCenter
    };

    QVector3D position() const;
    QVector3D upVector() const;
    QVector3D viewCenter() const;

    QVector3D viewVector() const;

    ProjectionType projectionType() const;

    void setOrthographicProjection( float left, float right,
                                    float bottom, float top,
                                    float nearPlane, float farPlane );

    void setPerspectiveProjection( float fieldOfView, float aspect,
                                   float nearPlane, float farPlane );

    void setNearPlane( const float& nearPlane );
    float nearPlane() const;

    void setFarPlane( const float& nearPlane );
    float farPlane() const;

    void setFieldOfView( const float& fieldOfView );
    float fieldOfView() const;

    void setAspectRatio( const float& aspectRatio );
    float aspectRatio() const;

    void setLeft( const float& left );
    float left() const;

    void setRight( const float& right );
    float right() const;

    void setBottom( const float& bottom );
    float bottom() const;

    void setTop( const float& top );
    float top() const;

    QMatrix4x4 viewMatrix() const;
    QMatrix4x4 projectionMatrix() const;
    QMatrix4x4 viewProjectionMatrix() const;

    QQuaternion tiltRotation( const float& angle ) const;
    QQuaternion panRotation( const float& angle ) const;
    QQuaternion panRotation( const float& angle, const QVector3D& axis ) const;
    QQuaternion rollRotation( const float& angle ) const;

    QVector<LogicalPlane> frustumClipPlanes() const;

    /**
     * @brief setStandardUniforms - set the standard transform uniforms on
     * the provided shader program. Standard names are 'mvp', 'modelViewMatrix',
     * 'normalMatrix' and 'projectionMatrix'
     * @param program - the program whose uniforms should be modified
     * @param model - the model matrix to use
     */
    void setStandardUniforms( const QOpenGLShaderProgramPtr& program, const QMatrix4x4& model ) const;

    /**
     * @brief setStandardUniforms - set the standard transform uniforms on
     * the provided shader program. Standard names are 'mvp', 'modelViewMatrix',
     * 'normalMatrix' and 'projectionMatrix'
     *
     * This overload changes the uniform values display only an area around a
     * focus point expanded to fill completely the viewport.
     *
     * This can be useful for picking.
     *
     * @param program - the program whose uniforms should be modified
     * @param model - the model matrix to use
     * @param viewport - the coordinates of the current viewport
     * @param p - the focus point we want to display
     * @param delta - the amount of space we want to display around the center point
     */
    void setStandardUniforms( const QOpenGLShaderProgramPtr& program, const QMatrix4x4& model,
                              const QRectF& viewport, const QPointF& p, const QSizeF& delta ) const;

public slots:
    void setPosition( const QVector3D& position );
    void setUpVector( const QVector3D& upVector );
    void setViewCenter( const QVector3D& viewCenter );

    void resetViewToIdentity();

    // Translate relative to camera orientation axes
    void translate( const QVector3D& vLocal, CameraTranslationOption option = TranslateViewCenter );

    // Translate relative to world axes
    void translateWorld( const QVector3D& vWorld, CameraTranslationOption option = TranslateViewCenter );

    void tilt( const float& angle );
    void pan( const float& angle );
    void pan( const float& angle, const QVector3D& axis );
    void roll( const float& angle );

    void tiltAboutViewCenter( const float& angle );
    void panAboutViewCenter( const float& angle );
    void panAboutViewCenter( const float& angle, const QVector3D& axis );
    void rollAboutViewCenter( const float& angle );

    void rotate( const QQuaternion& q );
    void rotateAboutViewCenter( const QQuaternion& q );

protected:
    Q_DECLARE_PRIVATE( Camera )

private:
    CameraPrivate* d_ptr;
};

#endif // CAMERA_H
