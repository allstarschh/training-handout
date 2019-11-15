#include "skybox.h"

#include <QImage>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLPixelTransferOptions>
#include <QOpenGLTexture>

#ifndef GL_TEXTURE_CUBE_MAP_SEAMLESS
#define GL_TEXTURE_CUBE_MAP_SEAMLESS 0x884F
#endif

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

// Indices
//
// 3 indices per triangle
// 2 triangles per face
// 6 faces
static const int indexDataCount = 6 * 3 * 2;

static const unsigned int indexData[indexDataCount] = {
    0,  1,  2,  0,  2,  3,  // Left face
    4,  5,  6,  4,  6,  7,  // Top face
    8,  9,  10, 8,  10, 11, // Right face
    12, 14, 15, 12, 13, 14, // Bottom face
    16, 17, 18, 16, 18, 19, // Front face
    20, 22, 23, 20, 21, 22  // Back face
};

SkyBox::SkyBox( QObject* parent )
    : QObject( parent ),
      m_seamlessCubeMap( true ),
      m_positionBuffer( QOpenGLBuffer::VertexBuffer ),
      m_indexBuffer( QOpenGLBuffer::IndexBuffer )
{
}

void SkyBox::setMaterial( const MaterialPtr& material )
{
    m_material = material;
}

void SkyBox::create()
{
    // Allocate some storage to hold per-vertex data
    float* v = new float[vertexDataCount];               // Vertices
    unsigned int* el = new unsigned int[indexDataCount]; // Elements
    m_indexCount = indexDataCount;

    // Generate the vertex data
    memcpy( v, vertexData, vertexDataCount * sizeof( float ) );
    memcpy( el, indexData, indexDataCount * sizeof( unsigned int ) );

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

    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern( QOpenGLBuffer::StaticDraw );
    m_indexBuffer.bind();
    m_indexBuffer.allocate( el, indexDataCount * sizeof( unsigned int ) );

    m_vao.release();

    // Delete our copy of the data as we no longer need it
    delete [] v;
    delete [] el;

    updateVertexArrayObject();
}

void SkyBox::updateVertexArrayObject()
{
    // Ensure that we have a valid material and geometry
    if ( !m_material || !m_positionBuffer.isCreated() )
        return;

    m_vao.bind();
    setupVertexArrays();
    m_vao.release();
}

void SkyBox::setupVertexArrays()
{
    QOpenGLShaderProgramPtr shader = m_material->shader();
    shader->bind();

    m_positionBuffer.bind();
    shader->enableAttributeArray( "vertexPosition" );
    shader->setAttributeBuffer( "vertexPosition", GL_FLOAT, 0, 3 );
    m_positionBuffer.release();

    m_indexBuffer.bind();
}

void SkyBox::render()
{
    m_vao.bind();
    QOpenGLFunctions* m_funcs= QOpenGLContext::currentContext()->functions();
    m_funcs->glDrawElements( GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr );
    m_vao.release();
}

void SkyBox::setCubeMapBaseName( const QString& cubeMapBaseName )
{
    m_cubeMapBaseName = cubeMapBaseName;
    if ( isCreated() )
        updateMaterialCubeMap();
}

void SkyBox::setSeamlessCubeMapEnabled( bool seamlessEnabled )
{
    m_seamlessCubeMap = seamlessEnabled;
}

void SkyBox::updateMaterialCubeMap()
{
    if ( !m_material )
        return;

    // Setup the texture image data - 6 faces for cubemap
    const QStringList suffixes = ( QStringList() << "posx" << "negx" << "posy" << "negy" << "posz" << "negz" );
    QOpenGLTexture::CubeMapFace targets[] =
    {
        QOpenGLTexture::CubeMapPositiveX, QOpenGLTexture::CubeMapNegativeX,
        QOpenGLTexture::CubeMapPositiveY, QOpenGLTexture::CubeMapNegativeY,
        QOpenGLTexture::CubeMapPositiveZ, QOpenGLTexture::CubeMapNegativeZ
    };

    QOpenGLTexturePtr cubeMapTexture( new QOpenGLTexture( QOpenGLTexture::TargetCubeMap ) );
    cubeMapTexture->setFormat( QOpenGLTexture::RGBA8_UNorm );
    cubeMapTexture->bind();
    for ( int i = 0; i < suffixes.size(); ++i )
    {
        QString fileName = QString( m_cubeMapBaseName ) + "_" + suffixes[i] + ".png";
        QImage image( fileName );
        QImage glImage = image.convertToFormat( QImage::Format_RGBA8888 );

        if ( i == 0 )
        {
            cubeMapTexture->setSize( image.width(), image.height() );
            cubeMapTexture->allocateStorage();
        }

        QOpenGLPixelTransferOptions options;
        options.setAlignment( 1 );
        cubeMapTexture->setData( 0, 0, targets[ i ],
                                 QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, glImage.bits(),
                                 &options );
    }

    cubeMapTexture->generateMipMaps();
    cubeMapTexture->setWrapMode( QOpenGLTexture::ClampToEdge );
    cubeMapTexture->setMinMagFilters( QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear );

    // Setup texture unit 0 with the texture data and sampler
    m_material->setTextureUnitConfiguration( 0, cubeMapTexture );

    // Use the seamless cubemap extensions
    if ( m_seamlessCubeMap && QOpenGLContext::currentContext()->hasExtension( "GL_ARB_seamless_cube_map" ) ) {
        QOpenGLFunctions* m_funcs= QOpenGLContext::currentContext()->functions();
        m_funcs->glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
    }
}
