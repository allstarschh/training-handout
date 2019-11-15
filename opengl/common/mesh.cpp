#include "mesh.h"

#include "axisalignedboundingbox.h"
#include "objloader.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>

Mesh::Mesh( QObject* parent )
    : QObject( parent ),
      m_positionBuffer( QOpenGLBuffer::VertexBuffer ),
      m_normalBuffer( QOpenGLBuffer::VertexBuffer ),
      m_textureCoordBuffer( QOpenGLBuffer::VertexBuffer ),
      m_tangentBuffer( QOpenGLBuffer::VertexBuffer ),
      m_indexBuffer( QOpenGLBuffer::IndexBuffer ),
      m_vao(),
      m_indexCount( 0 ),
      m_material(),
      m_points(),
      m_boundingVolume( new AxisAlignedBoundingBox() )
{
}

Mesh::~Mesh()
{

}

void Mesh::setMeshData( const ObjLoader& loader )
{
    // Construct buffers for the vertices, normals, texCoord, tangents and indices
    m_points = loader.vertices();
    const QVector<QVector3D> normals = loader.normals();
    const QVector<QVector2D> texCoords = loader.textureCoordinates();
    const QVector<QVector4D> tangents = loader.tangents();

    const QVector<unsigned int> elements = loader.indices();
    m_indexCount = elements.size();

    // Create a vertex array object and bind it so we don't clobber
    // any currently bound VAO when we bind the index buffer below
    if ( !m_vao.isCreated() )
        m_vao.create();
    m_vao.bind();

    // Create and populate the buffer objects
    m_positionBuffer.create();
    m_positionBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_positionBuffer.bind();
    m_positionBuffer.allocate( m_points.constData(), m_points.count() * sizeof( QVector3D ) );

    m_normalBuffer.create();
    m_normalBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_normalBuffer.bind();
    m_normalBuffer.allocate( normals.constData(), normals.count() * sizeof( QVector3D ) );

    if ( !texCoords.isEmpty() )
    {
        m_textureCoordBuffer.create();
        m_textureCoordBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
        m_textureCoordBuffer.bind();
        m_textureCoordBuffer.allocate( texCoords.constData(), texCoords.count() * sizeof( QVector2D ) );
    }

    if ( !tangents.isEmpty() )
    {
        m_tangentBuffer.create();
        m_tangentBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
        m_tangentBuffer.bind();
        m_tangentBuffer.allocate( tangents.constData(), tangents.count() * sizeof( QVector4D ) );
    }

    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_indexBuffer.bind();
    m_indexBuffer.allocate( elements.constData(), elements.count() * sizeof( unsigned int ) );

    m_vao.release();

    // Calculate the bounding volume
    m_boundingVolume->update( m_points );

    setupVertexArrayObject();
}

void Mesh::render()
{
    // Bind the vertex array oobject to set up our vertex buffers and index buffer
    m_vao.bind();

    // Draw it!
    QOpenGLFunctions* m_funcs= QOpenGLContext::currentContext()->functions();
    m_funcs->glDrawElements( GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr );

    m_vao.release();
}

void Mesh::setupVertexArrayObject()
{
    // Ensure that we have a valid material and geometry
    if ( !m_material || !m_positionBuffer.isCreated() )
        return;

    m_vao.bind();
    setupVertexArrays();
    m_vao.release();
}

AxisAlignedBoundingBox* Mesh::boundingVolume() const
{
    return m_boundingVolume.data();
}

void Mesh::computeNormalLinesBuffer( const MaterialPtr& mat, double scale )
{
    int vertexCount  = m_points.size();
    float* v = new float[6 * vertexCount];
    float* vPtr = v;

    m_normalBuffer.bind();
    float* n = reinterpret_cast<float*>( m_normalBuffer.map( QOpenGLBuffer::ReadOnly ) );
    Q_ASSERT(n);
    for ( int vIndex = 0; vIndex < vertexCount; ++vIndex )
    {
        QVector3D pt = m_points[vIndex];
        *vPtr++ = pt.x();
        *vPtr++ = pt.y();
        *vPtr++ = pt.z();
        *vPtr++ = pt.x() + (*n++ * scale);
        *vPtr++ = pt.y() + (*n++ * scale);
        *vPtr++ = pt.z() + (*n++ * scale);
    }

    m_normalBuffer.unmap();

    m_normalLines.create();
    m_normalLines.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_normalLines.bind();
    m_normalLines.allocate( v, 6 * vertexCount * sizeof( float ) );

    m_vaoNormals.create();
    m_vaoNormals.bind();
    mat->shader()->enableAttributeArray( "vertexPosition" );
    mat->shader()->setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );
    m_vaoNormals.release();
    m_normalLines.release();

    delete[] v;
}

void Mesh::computeTangentLinesBuffer( const MaterialPtr& mat, double scale )
{
    if (!m_tangentBuffer.isCreated()) {
        qWarning() << "computeTangentLinesBuffer: no tangents on mesh";
        return;
    }

    int nVerts = m_points.size();
    float* v = new float[6 * nVerts];
    float* vPtr = v;

    m_tangentBuffer.bind();
    float* t = reinterpret_cast<float*>( m_tangentBuffer.map( QOpenGLBuffer::ReadOnly ) );

    Q_ASSERT(t);
    for ( int vIndex = 0; vIndex < nVerts; ++vIndex )
    {
        QVector3D pt = m_points[vIndex];
        *vPtr++ = pt.x();
        *vPtr++ = pt.y();
        *vPtr++ = pt.z();
        *vPtr++ = pt.x() + (*t++ * scale);
        *vPtr++ = pt.y() + (*t++ * scale);
        *vPtr++ = pt.z() + (*t++ * scale);
        t++; // skip fourth tangent value
    }

    m_tangentBuffer.bind();
    m_tangentBuffer.unmap();

    m_tangentLines.create();
    m_tangentLines.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_tangentLines.bind();
    m_tangentLines.allocate( v, 6 * nVerts * sizeof( float ) );

    m_vaoTangents.create();
    m_vaoTangents.bind();
    mat->shader()->enableAttributeArray( "vertexPosition" );
    mat->shader()->setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );
    m_vaoTangents.release();
    m_tangentLines.release();

    delete[] v;
}

void Mesh::renderNormalLines()
{
    m_vaoNormals.bind();
    QOpenGLFunctions* m_funcs= QOpenGLContext::currentContext()->functions();
    m_funcs->glDrawArrays( GL_LINES, 0, m_points.size() * 2 );
    m_vaoNormals.release();
}

void Mesh::renderTangentLines()
{
    m_vaoTangents.bind();
    QOpenGLFunctions* m_funcs= QOpenGLContext::currentContext()->functions();
    m_funcs->glDrawArrays( GL_LINES, 0, m_points.size() * 2 );
    m_vaoTangents.release();
}

void Mesh::setupVertexArrays()
{
    m_material->shader()->bind();

    m_positionBuffer.bind();
    m_material->shader()->enableAttributeArray( "vertexPosition" );
    m_material->shader()->setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );

    m_normalBuffer.bind();
    m_material->shader()->enableAttributeArray( "vertexNormal" );
    m_material->shader()->setAttributeBuffer( "vertexNormal", GL_FLOAT, 0, 3 );
    m_normalBuffer.release();

    if ( m_textureCoordBuffer.isCreated() )
    {
        m_textureCoordBuffer.bind();
        m_material->shader()->enableAttributeArray( "vertexTexCoord" );
        m_material->shader()->setAttributeBuffer( "vertexTexCoord", GL_FLOAT, 0, 2 );
        m_textureCoordBuffer.release();
    }

    if ( m_tangentBuffer.isCreated() )
    {
        m_tangentBuffer.bind();
        m_material->shader()->enableAttributeArray( "vertexTangent" );
        m_material->shader()->setAttributeBuffer( "vertexTangent", GL_FLOAT, 0, 4 );
        m_tangentBuffer.release();
    }

    m_indexBuffer.bind();
}

int Mesh::indexCount() const
{
    return m_indexCount;
}
