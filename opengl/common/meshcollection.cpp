#include "meshcollection.h"

#include "objloader.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions_4_3_Core>
#include <QStringList>

MeshCollection::MeshCollection( QObject* parent )
    : QObject( parent ),
      m_positionBuffer( QOpenGLBuffer::VertexBuffer ),
      m_normalBuffer( QOpenGLBuffer::VertexBuffer ),
      m_textureCoordBuffer( QOpenGLBuffer::VertexBuffer ),
      m_tangentBuffer( QOpenGLBuffer::VertexBuffer ),
      m_indexBuffer( QOpenGLBuffer::IndexBuffer ),
      m_vao(),
      m_funcs( nullptr )
{
}

void MeshCollection::setMeshData( const QStringList& fileNames )
{
    if (!m_funcs) {
        QOpenGLContext* context = QOpenGLContext::currentContext();
        Q_CHECK_PTR(context);
        m_funcs = context->versionFunctions<QOpenGLFunctions_4_3_Core>();
        Q_CHECK_PTR(m_funcs);

        m_vao.destroy();
        m_positionBuffer.destroy();
        m_normalBuffer.destroy();
        m_textureCoordBuffer.destroy();
        m_tangentBuffer.destroy();
        m_indexBuffer.destroy();
    }

    m_positionBuffer.create();
    m_positionBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_normalBuffer.create();
    m_normalBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_textureCoordBuffer.create();
    m_textureCoordBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_tangentBuffer.create();
    m_tangentBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );

    QVector<QVector3D> positions;
    QVector<QVector3D> normals;
    QVector<QVector2D> texCoords;
    QVector<QVector4D> tangents;
    QVector<unsigned int> indices;

    // Load vertex data to main memory
    foreach ( const QString& fileName, fileNames )
    {
        ObjLoader loader;
        loader.setLoadTextureCoordinatesEnabled( true );
        loader.setTangentGenerationEnabled( true );
        if ( loader.load( fileName ) )
        {
            // Store MeshInfo
            MeshInfo meshInfo;
            meshInfo.indexCount = loader.indices().size();
            meshInfo.firstIndex = indices.size();
            meshInfo.baseVertex = positions.size();
            m_meshInfo.append( meshInfo );

            // Concatenate with other mesh data
            positions += loader.vertices();
            normals   += loader.normals();
            texCoords += loader.textureCoordinates();
            tangents  += loader.tangents();
            indices   += loader.indices();
        }
    }

    const int vertexCount = positions.size();
    const int indexCount = indices.size();

    // Allocate sufficient storage and buffer data to GPU
    m_positionBuffer.bind();
    m_positionBuffer.allocate( positions.constData(), vertexCount * sizeof( QVector3D ) );

    m_normalBuffer.bind();
    m_normalBuffer.allocate( normals.constData(), vertexCount * sizeof( QVector3D ) );

    m_textureCoordBuffer.bind();
    m_textureCoordBuffer.allocate( texCoords.constData(), vertexCount * sizeof( QVector2D ) );

    m_tangentBuffer.bind();
    m_tangentBuffer.allocate( tangents.constData(), vertexCount * sizeof( QVector4D ) );

    m_indexBuffer.bind();
    m_indexBuffer.allocate( indices.constData(), indexCount * sizeof( unsigned int ) );

    setupVertexArrayObject();
}

void MeshCollection::setupVertexArrays()
{
    m_material->shader()->bind();

    m_positionBuffer.bind();
    m_material->shader()->enableAttributeArray( "vertexPosition" );
    m_material->shader()->setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );

    m_normalBuffer.bind();
    m_material->shader()->enableAttributeArray( "vertexNormal" );
    m_material->shader()->setAttributeBuffer( "vertexNormal", GL_FLOAT, 0, 3 );

    m_textureCoordBuffer.bind();
    m_material->shader()->enableAttributeArray( "vertexTexCoord" );
    m_material->shader()->setAttributeBuffer( "vertexTexCoord", GL_FLOAT, 0, 2 );

    m_tangentBuffer.bind();
    m_material->shader()->enableAttributeArray( "vertexTangent" );
    m_material->shader()->setAttributeBuffer( "vertexTangent", GL_FLOAT, 0, 4 );

    m_indexBuffer.bind();
}

void MeshCollection::render( GLuint commandBuffer,
                             int commandCount, int stride )
{
    m_vao.bind();
    m_funcs->glBindBuffer( GL_DRAW_INDIRECT_BUFFER, commandBuffer );
    m_funcs->glMultiDrawElementsIndirect( GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, commandCount, stride );
    m_funcs->glBindBuffer( GL_DRAW_INDIRECT_BUFFER, 0 );
    m_vao.release();
}

int MeshCollection::count() const
{
    return m_meshInfo.size();
}

GLuint MeshCollection::indexCount( int meshIndex ) const
{
    return m_meshInfo.at( meshIndex ).indexCount;
}

GLuint MeshCollection::firstIndex( int meshIndex ) const
{
    return m_meshInfo.at( meshIndex ).firstIndex;
}

GLint MeshCollection::baseVertex( int meshIndex ) const
{
    return m_meshInfo.at( meshIndex ).baseVertex;
}

void MeshCollection::setupVertexArrayObject()
{
    m_vao.create();
    m_vao.bind();

    setupVertexArrays();

    m_vao.release();
    m_indexBuffer.release();
    m_tangentBuffer.release();
}
