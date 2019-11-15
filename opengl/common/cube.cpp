#include "cube.h"

#include "axisalignedboundingbox.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

// Vertices
//
// 3 floats per vertex
// 4 vertices per face
// 6 faces
static const int vertexDataCount = 6 * 4 * 3;

static const float vertexData[vertexDataCount] = {
    // Left face
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,

    // Top face
    -0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f,  0.5f,
     0.5f, 0.5f,  0.5f,
     0.5f, 0.5f, -0.5f,

    // Right face
    0.5f,  0.5f, -0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f, -0.5f, -0.5f,

    // Bottom face
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    // Front face
     0.5f, -0.5f, 0.5f,
     0.5f,  0.5f, 0.5f,
    -0.5f,  0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,

    // Back face
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f
};


// Normal vectors
static const int normalDataCount = 6 * 4 * 3;

static const float normalData[normalDataCount] = {
    // Left face
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,

    // Top face
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,

    // Right face
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,

    // Bottom face
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,

    // Front face
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,

    // Back face
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f
};


// Texure coords
static const int textureCoordDataCount = 6 * 4 * 2;

static const float textureCoordData[textureCoordDataCount] = {
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,

    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,

    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,

    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,

    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,

    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f
};


static const int tangentDataCount = 6 * 4 * 3;

static const float tangentData[tangentDataCount] = {
    // Left face
     0.0f, -1.0f,  0.0f,
     0.0f, -1.0f,  0.0f,
     0.0f, -1.0f,  0.0f,
     0.0f, -1.0f,  0.0f,

    // Top face
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,

    // Right face
     0.0f,  1.0f,  0.0f,
     0.0f,  1.0f,  0.0f,
     0.0f,  1.0f,  0.0f,
     0.0f,  1.0f,  0.0f,

    // Bottom face
     1.0f,  0.0f,  0.0f,
     1.0f,  0.0f,  0.0f,
     1.0f,  0.0f,  0.0f,
     1.0f,  0.0f,  0.0f,

    // Front face
     1.0f,  0.0f,  0.0f,
     1.0f,  0.0f,  0.0f,
     1.0f,  0.0f,  0.0f,
     1.0f,  0.0f,  0.0f,

    // Back face
     1.0f,  0.0f,  0.0f,
     1.0f,  0.0f,  0.0f,
     1.0f,  0.0f,  0.0f,
     1.0f,  0.0f,  0.0f
};


// Indices
//
// 3 indices per triangle
// 2 triangles per face
// 6 faces
static const int trianglesIndexDataCount = 6 * 3 * 2;

static const unsigned int trianglesIndexData[trianglesIndexDataCount] = {
    0,  1,  2,  0,  2,  3,  // Left face
    4,  5,  6,  4,  6,  7,  // Top face
    8,  9,  10, 8,  10, 11, // Right face
    12, 14, 15, 12, 13, 14, // Bottom face
    16, 17, 18, 16, 18, 19, // Front face
    20, 22, 23, 20, 21, 22  // Back face
};

/*
 * A triangle adjacency needs indexes in this order:
 *
 * 1--2--3
 *  \ |\ |
 *   \| \|
 *    0--4
 *     \ |
 *      \|
 *       5
 *
 * 0, 2, 4 gets drawn
 *
 * 0, 1, 2 }
 * 2, 3, 4 } adjacency information
 * 0, 4, 5 }
*/

// 6 faces, 2 triangles per face, 6 vertices per triangle (3 for triangle itself + 3 for adj information)
static const int trianglesAdjacencyIndexDataCount = 6 * 2 * 6;
static const unsigned int trianglesAdjacencyIndexData[trianglesAdjacencyIndexDataCount] = {
    // left face
    0,  12,  1,  16, 2,  3,
    0,   1,  2,  6,  3, 20,

    // top face
    4,   0,  5, 16,  6,  7,
    4,   5,  6, 10,  7, 22,

    // right face
    8,   4,  9, 18, 10, 11,
    8,   9, 10, 14, 11, 22,

    // bottom face
    12, 13, 14,  2, 15, 20,
    12,  8, 13, 18, 14, 15,

    // front face
    16,  8, 17,  4, 18, 19,
    16, 17, 18,  0, 19, 12,

    // back face
    20, 21, 22,  2, 23, 6,
    20, 10, 21, 14, 22, 23
};

Cube::Cube( QObject *parent )
    : QObject( parent ),
      m_drawMode( TrianglesDrawMode ),
      m_lengths( 1.0, 1.0, 1.0 ),
      m_positionBuffer( QOpenGLBuffer::VertexBuffer ),
      m_normalBuffer( QOpenGLBuffer::VertexBuffer ),
      m_textureCoordBuffer( QOpenGLBuffer::VertexBuffer ),
      m_tangentBuffer( QOpenGLBuffer::VertexBuffer ),
      m_indexBuffer( QOpenGLBuffer::IndexBuffer ),
      m_vao()
{
}

Cube::Cube( DrawMode drawMode, QObject *parent )
    : QObject( parent ),
      m_drawMode( drawMode ),
      m_lengths( 1.0, 1.0, 1.0 ),
      m_positionBuffer( QOpenGLBuffer::VertexBuffer ),
      m_normalBuffer( QOpenGLBuffer::VertexBuffer ),
      m_textureCoordBuffer( QOpenGLBuffer::VertexBuffer ),
      m_tangentBuffer( QOpenGLBuffer::VertexBuffer ),
      m_indexBuffer( QOpenGLBuffer::IndexBuffer ),
      m_vao()
{
}

void Cube::setMaterial( const MaterialPtr& material )
{
    if ( material == m_material )
        return;
    m_material = material;
    updateVertexArrayObject();
}

MaterialPtr Cube::material() const
{
    return m_material;
}

int Cube::indexCount() const
{
    switch (m_drawMode) {
    case Cube::TrianglesDrawMode:
        return trianglesIndexDataCount;
    case Cube::TrianglesAdjacencyMode:
        return trianglesAdjacencyIndexDataCount;
    }

    Q_UNREACHABLE();
    return 0;
}

QVector3D Cube::sideLengths() const
{
    return m_lengths;
}

QVector<QVector3D> Cube::axisAlignedBoundingBoxPoints() const
{
    QVector<QVector3D> points;

    points.append( QVector3D(  m_lengths[0] / 2,  m_lengths[1] / 2,  m_lengths[2] / 2 ) );
    points.append( QVector3D( -m_lengths[0] / 2, -m_lengths[1] / 2, -m_lengths[2] / 2 ) );

    return points;
}

AxisAlignedBoundingBox Cube::axisAlignedBoundingBox() const
{
    return AxisAlignedBoundingBox( axisAlignedBoundingBoxPoints() );
}

void Cube::setLength(float arg)
{
    m_lengths = QVector3D(arg, arg, arg);
}

void Cube::create()
{
    // Allocate some storage to hold per-vertex data
    float* v = new float[vertexDataCount];               // Vertices
    float* n = new float[normalDataCount];               // Normals
    float* tex = new float[textureCoordDataCount];       // Tex coords
    float* t = new float[tangentDataCount];              // Tangents
    unsigned int* el = new unsigned int[indexCount()];   // Elements

    // Generate the vertex data
    generateVertexData( v, n, tex, t, el );

    // Create a vertex array object and bind it so we don't clobber
    // any currently bound VAO when we bind the index buffer below
    if ( !m_vao.isCreated() )
        m_vao.create();
    m_vao.bind();

    // Create and populate the buffer objects
    m_positionBuffer.create();
    m_positionBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_positionBuffer.bind();
    m_positionBuffer.allocate( v, vertexDataCount * sizeof( float ) );

    m_normalBuffer.create();
    m_normalBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_normalBuffer.bind();
    m_normalBuffer.allocate( n, normalDataCount * sizeof( float ) );

    m_textureCoordBuffer.create();
    m_textureCoordBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_textureCoordBuffer.bind();
    m_textureCoordBuffer.allocate( tex, textureCoordDataCount * sizeof( float ) );

    m_tangentBuffer.create();
    m_tangentBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_tangentBuffer.bind();
    m_tangentBuffer.allocate( t, tangentDataCount * sizeof( float ) );

    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_indexBuffer.bind();
    m_indexBuffer.allocate( el, indexCount() * sizeof( unsigned int ) );

    m_vao.release();

    // Delete our copy of the data as we no longer need it
    delete [] v;
    delete [] n;
    delete [] el;
    delete [] tex;
    delete [] t;

    updateVertexArrayObject();
}

void Cube::generateVertexData( float* vertices, float* normals, float* texCoords, float* tangents, unsigned int* indices )
{
    for (int i = 0; i < vertexDataCount; ++i)
        vertices[i] = vertexData[i] * m_lengths[i % 3];

    memcpy( normals, normalData, normalDataCount * sizeof( float ) );
    memcpy( texCoords, textureCoordData, textureCoordDataCount * sizeof( float ) );
    memcpy( tangents, tangentData, tangentDataCount * sizeof( float ) );

    switch (m_drawMode) {
    case Cube::TrianglesDrawMode:
        memcpy( indices, trianglesIndexData, indexCount() * sizeof( unsigned int ) );
        break;
    case Cube::TrianglesAdjacencyMode:
        memcpy( indices, trianglesAdjacencyIndexData, indexCount() * sizeof( unsigned int ) );
        break;
    }
}

void Cube::updateVertexArrayObject()
{
    // Ensure that we have a valid material and geometry
    if ( !m_material || !m_positionBuffer.isCreated() )
        return;

    m_vao.bind();
    setupVertexArrays();
    m_vao.release();
}

void Cube::render()
{
    // Bind the vertex array oobject to set up our vertex buffers and index buffer
    m_vao.bind();

    QOpenGLFunctions *functions = QOpenGLContext::currentContext()->functions();

    // Draw it!
    switch (m_drawMode) {
    case Cube::TrianglesDrawMode:
        functions->glDrawElements( GL_TRIANGLES, indexCount(), GL_UNSIGNED_INT, nullptr );
        break;
    case Cube::TrianglesAdjacencyMode:
#if !defined(QT_OPENGL_ES_2)
        functions->glDrawElements( GL_TRIANGLES_ADJACENCY, indexCount(), GL_UNSIGNED_INT, nullptr );
#endif
        break;
    }

    m_vao.release();
}

void Cube::setSideLengths(const QVector3D &arg)
{
    m_lengths = arg;
}

void Cube::setupVertexArrays()
{
    QOpenGLShaderProgramPtr shader = m_material->shader();
    shader->bind();

    m_positionBuffer.bind();
    shader->enableAttributeArray( "vertexPosition" );
    shader->setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );

    m_normalBuffer.bind();
    shader->enableAttributeArray( "vertexNormal" );
    shader->setAttributeBuffer( "vertexNormal", GL_FLOAT, 0, 3 );

    m_textureCoordBuffer.bind();
    shader->enableAttributeArray( "vertexTexCoord" );
    shader->setAttributeBuffer( "vertexTexCoord", GL_FLOAT, 0, 2 );

    m_tangentBuffer.bind();
    shader->enableAttributeArray( "vertexTangent" );
    shader->setAttributeBuffer( "vertexTangent", GL_FLOAT, 0, 3 );
    m_tangentBuffer.release();

    m_indexBuffer.bind();
}

