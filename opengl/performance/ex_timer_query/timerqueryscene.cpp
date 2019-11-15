#include <QOpenGLContext>
#include <QOpenGLFunctions>

#include "timerqueryscene.h"

#include <camera.h>
#include <mesh.h>
#include <objloader.h>

#include <QImage>
#include <QOpenGLTimeMonitor>

TimerQueryScene::TimerQueryScene( QObject* parent )
    : CameraScene( parent ),
      m_mesh( nullptr )
{
    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 0.0f, 0.0f, 3.0f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );
}

void TimerQueryScene::initialise()
{
    m_funcs = QOpenGLContext::currentContext()->functions();

    // Create a material that performs multi-texturing
    MaterialPtr material = createTextureMaterial(
                               ":/meshes/ogrehead/ogrehead_diffuse.png",
                               ":/meshes/ogrehead/ogre_normalmap.png" );

    // Load the model mesh
    ObjLoader loader;
    loader.setLoadTextureCoordinatesEnabled( true );
    loader.setTangentGenerationEnabled( true );
    loader.load( ":/meshes/ogrehead/ogrehead.obj" );

    m_mesh = new Mesh( this );
    m_mesh->setMaterial( material );
    m_mesh->setMeshData( loader );

    // Enable depth testing
    m_funcs->glEnable( GL_DEPTH_TEST );

    // Set the clear color to white
    m_funcs->glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

    // Create a timer query object
    m_timeMonitor = new QOpenGLTimeMonitor( this );
    m_timeMonitor->setSampleCount( 5 );
    if ( !m_timeMonitor->create() )
        qWarning() << "Failed to create timer query object";

    // Generate some names for the various stages of rendering
    m_renderStages << "Clear Buffer" << "Material Binding" << "Set Uniforms" << "Drawing";
}

void TimerQueryScene::update( float t )
{
    CameraScene::update( t );
    m_modelMatrix.setToIdentity();
    m_modelMatrix.rotate( modelTheta(), 0.0f, 1.0f, 0.0f );
}

void TimerQueryScene::render()
{
    // Start the timer query
    m_timeMonitor->recordSample();

    // Clear the buffer with the current clearing color
    m_funcs->glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_timeMonitor->recordSample();

    m_mesh->material()->bind();
    QOpenGLShaderProgramPtr shader = m_mesh->material()->shader();

    m_timeMonitor->recordSample();

    m_camera->setStandardUniforms( shader, m_modelMatrix );

    // Set the lighting parameters
    shader->setUniformValue( "light.position", QVector4D( 0.0f, 0.0f, 0.0f, 1.0f ) );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );
    shader->setUniformValue( "light.ac", 1.0f );
    shader->setUniformValue( "light.al", 0.0f );
    shader->setUniformValue( "light.aq", 0.0f );

    // Set the material properties
    shader->setUniformValue( "material.Ka", QVector3D( 0.1f, 0.1f, 0.1f ) );
    shader->setUniformValue( "material.Kd", QVector3D( 0.8f, 0.8f, 0.8f ) );
    shader->setUniformValue( "material.Ks", QVector3D( 0.1f, 0.1f, 0.1f ) );
    shader->setUniformValue( "material.shininess", 20.0f );

    m_timeMonitor->recordSample();

    // Let the mesh setup the remainder of its state and draw itself
    m_mesh->render();
    m_timeMonitor->recordSample();

    // Block until the results are available
    QVector<GLuint64> intervals = m_timeMonitor->waitForIntervals();
    QVector<GLuint64> timeSamples = m_timeMonitor->waitForSamples();
    qDebug() << "timeSamples =" << timeSamples;
    for ( int i = 0; i < intervals.count(); ++i )
        qDebug() << i << m_renderStages.at( i ) << double( intervals.at( i ) ) / 1.0e6 << "msecs";

    // Clear the query results ready for the next frame
    m_timeMonitor->reset();
}

MaterialPtr TimerQueryScene::createTextureMaterial( const QString& diffuse,
                                                   const QString& normal )
{
    // Create a material and set the shaders
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/normalmap.vert",
                          ":/shaders/normalmap.frag" );

    // Create a diffuse texture
    QOpenGLTexturePtr texture0( new QOpenGLTexture( QImage( diffuse ).mirrored() ) );
    texture0->setWrapMode( QOpenGLTexture::ClampToEdge );
    texture0->setMinMagFilters( QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear );

    // Create a normal map
    QOpenGLTexturePtr texture1( new QOpenGLTexture( QImage( normal ).mirrored() ) );
    texture1->setWrapMode( QOpenGLTexture::ClampToEdge );
    texture1->setMinMagFilters( QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear );

    // Associate the textures with the first 2 texture units
    material->setTextureUnitConfiguration( 0, texture0, QByteArrayLiteral( "texture0" ) );
    material->setTextureUnitConfiguration( 1, texture1, QByteArrayLiteral( "texture1" ) );

    return material;
}
