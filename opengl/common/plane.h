#ifndef PLANE_H
#define PLANE_H

#include <QObject>

#include "material.h"

#include <QOpenGLBuffer>
#include "qopenglvertexarrayobject.h"

class AxisAlignedBoundingBox;

class Plane : public QObject
{
    Q_OBJECT
    Q_PROPERTY( float width READ width WRITE setWidth )
    Q_PROPERTY( float height READ height WRITE setHeight )
    Q_PROPERTY( int xDivisions READ xDivisions WRITE setXDivisions )
    Q_PROPERTY( int yDivisions READ yDivisions WRITE setYDivisions )

public:
    explicit Plane( QObject* parent = nullptr );
    Plane( float width, float height, int xDivisions, int yDivisions, QObject* parent = nullptr );

    void setMaterial( const MaterialPtr& material );
    MaterialPtr material() const;
    
    float width() const
    {
        return m_width;
    }

    float height() const
    {
        return m_height;
    }

    int xDivisions() const
    {
        return m_xDivisions;
    }

    int yDivisions() const
    {
        return m_yDivisions;
    }

    int indexCount() const { return 6 * m_xDivisions * m_yDivisions; }

    QVector<QVector3D> axisAlignedBoundingBoxPoints() const;
    AxisAlignedBoundingBox axisAlignedBoundingBox() const;

    void updateVertexArrayObject();
    void setupVertexArrays();

public slots:
    void setWidth(float width)
    {
        m_width = width;
    }

    void setHeight(float height)
    {
        m_height = height;
    }

    void setXDivisions(int xDivisions)
    {
        m_xDivisions = xDivisions;
    }

    void setYDivisions(int yDivisions)
    {
        m_yDivisions = yDivisions;
    }

    void create();

    void render();

private:
    void generateVertexData( QVector<float>& vertices,
                             QVector<float>& normals,
                             QVector<float>& texCoords,
                             QVector<float>& tangents,
                             QVector<unsigned int>& indices );

    float m_width;
    float m_height;
    int m_xDivisions;
    int m_yDivisions;

    MaterialPtr m_material;

    // QVertexBuffers to hold the data
    QOpenGLBuffer m_positionBuffer;
    QOpenGLBuffer m_normalBuffer;
    QOpenGLBuffer m_textureCoordBuffer;
    QOpenGLBuffer m_tangentBuffer;
    QOpenGLBuffer m_indexBuffer;

    QOpenGLVertexArrayObject m_vao;
};

#endif // PLANE_H
