#ifndef MESHCOLLECTION_H
#define MESHCOLLECTION_H

#include <QObject>

#include "indirectcommands.h"
#include "material.h"

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVector>

class QOpenGLFunctions_4_3_Core;

class MeshCollection : public QObject
{
    Q_OBJECT
public:
    explicit MeshCollection( QObject* parent = nullptr );

    void setMeshData( const QStringList& fileNames );

    void setMaterial( const MaterialPtr& material ) { m_material = material; }
    MaterialPtr material() const { return m_material; }

    void setupVertexArrays();

    void render( GLuint commandBuffer,
                 int commandCount, int stride = 0 );

    int count() const;
    GLuint indexCount( int meshIndex ) const;
    GLuint firstIndex( int meshIndex ) const;
    GLint baseVertex( int meshIndex ) const;

    QOpenGLVertexArrayObject* vertexArrayObject() { return &m_vao; }

private:
    void setupVertexArrayObject();

    // Buffers to hold the data
    QOpenGLBuffer m_positionBuffer;
    QOpenGLBuffer m_normalBuffer;
    QOpenGLBuffer m_textureCoordBuffer;
    QOpenGLBuffer m_tangentBuffer;
    QOpenGLBuffer m_indexBuffer;

    // Vertex array object to quickly set render state
    QOpenGLVertexArrayObject m_vao;

    MaterialPtr m_material;

    // Info about the locations and sizes of meshes in the
    // combined buffers. This needs to be available so that
    // clients can correctly construct indirect draw commands
    struct MeshInfo
    {
        GLuint indexCount;
        GLuint firstIndex;
        GLint baseVertex;
    };
    QVector<MeshInfo> m_meshInfo;

    QOpenGLFunctions_4_3_Core* m_funcs;
};

#endif // MESHCOLLECTION_H
