#ifndef AXISGNOMON_H
#define AXISGNOMON_H

#include <QObject>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include "material.h"

class Camera;


class AxisGnomon : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int size READ size WRITE setSize)
    Q_PROPERTY(QSize windowSize READ windowSize WRITE setWindowSize)
    Q_PROPERTY(QPoint position READ position WRITE setPosition)

public:
    explicit AxisGnomon(QObject *parent = nullptr);
    AxisGnomon(int cylinderSides, QObject *parent = nullptr);

    int size() const;
    void setSize(int size);

    QSize windowSize() const;
    void setWindowSize(QSize size);

    QPoint position() const;
    void setPosition(QPoint position);

    int indexCount() const;

    void render(Camera *camera);

private:
    void create();
    void generateVertexData(QVector<float>& vertices,
                             QVector<float> &color,
                             QVector<unsigned int>& indices );
    void updateProjectionMatrix();

    const int m_cylinderSides;

    int m_size;
    QSize m_windowSize;
    QPoint m_position;

    QMatrix4x4 m_projection;

    MaterialPtr m_material;

    QOpenGLBuffer m_positionBuffer;
    QOpenGLBuffer m_colorBuffer;
    QOpenGLBuffer m_indexBuffer;

    QOpenGLVertexArrayObject m_vao;
};

#endif // AXISGNOMON_H
