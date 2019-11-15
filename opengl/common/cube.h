#ifndef CUBE_H
#define CUBE_H

#include <QObject>

#include "material.h"

#include <QOpenGLBuffer>
#include "qopenglvertexarrayobject.h"

class QOpenGLShaderProgram;
class AxisAlignedBoundingBox;

class Cube : public QObject
{
    Q_OBJECT
    Q_PROPERTY( float length READ length WRITE setLength )

    Q_PROPERTY( QVector3D sideLengths READ sideLengths WRITE setSideLengths )
public:
    enum DrawMode {
        TrianglesDrawMode,
        TrianglesAdjacencyMode
    };

    explicit Cube( QObject* parent = nullptr );

    explicit Cube( DrawMode mode, QObject* parent = nullptr );

    void setMaterial( const MaterialPtr& material );
    MaterialPtr material() const;

    float length() const
    {
        return m_lengths[0];
    }

    QOpenGLVertexArrayObject* vertexArrayObject() { return &m_vao; }


    int indexCount() const;

    QVector3D sideLengths() const;

    QVector<QVector3D> axisAlignedBoundingBoxPoints() const;
    AxisAlignedBoundingBox axisAlignedBoundingBox() const;

    void create();

    void updateVertexArrayObject();
    void setupVertexArrays();

public slots:
    void setLength( float arg );

    void render();

    void setSideLengths( const QVector3D &arg );

private:
    void generateVertexData(float* vertices, float* normals, float* texCoords, float* tangents, unsigned int* indices );

    MaterialPtr m_material;
    DrawMode m_drawMode;
    QVector3D m_lengths;

    // QVertexBuffers to hold the data
    QOpenGLBuffer m_positionBuffer;
    QOpenGLBuffer m_normalBuffer;
    QOpenGLBuffer m_textureCoordBuffer;
    QOpenGLBuffer m_tangentBuffer;
    QOpenGLBuffer m_indexBuffer;

    QOpenGLVertexArrayObject m_vao;
};

#endif // CUBE_H
