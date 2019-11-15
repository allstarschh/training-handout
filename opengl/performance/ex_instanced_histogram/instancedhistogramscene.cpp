#include <QOpenGLFunctions>

#include "instancedhistogramscene.h"

#include <camera.h>
#include <cube.h>
#include <torus.h>
#include <material.h>
#include <mathutils.h>
#include <sphere.h>

#include <QObject>
#include <QOpenGLContext>
#include <QOpenGLFunctions_3_3_Core>

static const int xPoints = 100;
static const int zPoints = 100;
static const int pointCount = xPoints * zPoints;

InstancedHistogramScene::InstancedHistogramScene( QObject* parent )
    : CameraScene( parent ),
      m_funcs( nullptr ),
      m_theta( 0.0f ),
      m_modelMatrix(),
      m_data( 3 * pointCount ),
      m_sphere( nullptr ),
      m_sphereModelMatrix(),
      m_time( 0.0f ),
      m_updatesEnabled( true )
{
    m_modelMatrix.setToIdentity();
    update( 0.0f );

    // Initialize the camera position and orientation
    m_camera->setPosition( QVector3D( 0.0f, 0.0f, 25.0f ) );
    m_camera->setViewCenter( QVector3D( 0.0f, 0.0f, 0.0f ) );
    m_camera->setUpVector( QVector3D( 0.0f, 1.0f, 0.0f ) );

    m_camera->translateWorld( QVector3D( 0.0f, 7.0f, 0.0f ), Camera::DontTranslateViewCenter );
}

void InstancedHistogramScene::initialise()
{
    // Resolve the OpenGL functions that we need for instanced rendering
    m_funcs = m_context->versionFunctions<QOpenGLFunctions_3_3_Core>();
    if ( !m_funcs )
        qFatal( "Could not obtain required OpenGL context version" );
    m_funcs->initializeOpenGLFunctions();

    // Create a Material we can use to operate on instanced geometry
    MaterialPtr material( new Material );
    material->setShaders( ":/shaders/instancedhistogram.vert",
                          ":/shaders/instancedhistogram.frag" );

    // Create a cube
    m_cube = new Cube( this );
    m_cube->setMaterial( material );
    m_cube->create();

    // Create a VBO ready to hold our data
    prepareVertexBuffers();

    // Tell OpenGL how to pass the data VBOs to the shader program
    prepareVertexArrayObject();

    // Use a sphere for the background - we'll render the scene
    // inside the sphere
    MaterialPtr noiseMaterial( new Material );
    noiseMaterial->setShaders( ":/shaders/noise.vert",
                               ":/shaders/noise.frag" );

    m_sphere = new Sphere( this );
    m_sphere->setMaterial( noiseMaterial );
    m_sphere->create();

    // Enable depth testing to prevent artifacts
    m_funcs->glEnable( GL_DEPTH_TEST );
}

void InstancedHistogramScene::update( float t )
{
    // Update the data being plotted
    if ( m_updatesEnabled )
    {
        const float xMin = -15.0f, xMax = 15.0f;
        const float zMin = -15.0f, zMax = 15.0f;
        const float dx = ( xMax - xMin ) / static_cast<float>( xPoints - 1 );
        const float dz = ( zMax - zMin ) / static_cast<float>( zPoints - 1 );
        int i = 0;
        const float A = 5.0;
        for ( int zi = 0; zi < zPoints; ++zi )
        {
            float z = zMin + static_cast<float>( zi ) * dz;

            for ( int xi = 0; xi < xPoints; ++xi )
            {
                float x = xMin + static_cast<float>( xi ) * dx;

                double r = sqrt( x * x + z * z );
                float y = A * ( sinf( 2.5f * t ) + cosf( 2.5f * t ) ) * bessel_j0( r );

                m_data[3 * i]     = x;
                m_data[3 * i + 1] = y;
                m_data[3 * i + 2] = z;
                ++i;
            }
        }
    }

    // Store the time to pass into the shader
    m_time = t;

    m_sphereModelMatrix.setToIdentity();
    m_sphereModelMatrix.scale( 30.0f );
}

void InstancedHistogramScene::render()
{
    m_funcs->glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_dataBuffer.bind();
    m_dataBuffer.allocate( m_data.data(), m_data.size() * sizeof( float ) );

    // Bind the shader program
    m_cube->material()->bind();
    QOpenGLShaderProgramPtr shader = m_cube->material()->shader();

    // Calculate needed matrices
    m_modelMatrix.setToIdentity();
    m_modelMatrix.rotate( m_theta, 0.0f, 1.0f, 0.0f );

    m_camera->setStandardUniforms( shader, m_modelMatrix );

    // Scale the x-z dimensions of the cuboids
    shader->setUniformValue( "cubeScale", QVector2D( 0.25f, 0.25f ) );

    // Set the lighting parameters
    shader->setUniformValue( "light.position", QVector4D( 0.0f, 0.0f, 0.0f, 1.0f ) );
    shader->setUniformValue( "light.intensity", QVector3D( 1.0f, 1.0f, 1.0f ) );
    shader->setUniformValue( "light.ac", 1.0f );
    shader->setUniformValue( "light.al", 0.0f );
    shader->setUniformValue( "light.aq", 0.0f );
    shader->setUniformValue( "material.ks", QVector3D( 0.1f, 0.1f, 0.1f ) );
    shader->setUniformValue( "material.ka", QVector3D( 0.1f, 0.1f, 0.1f ) );
    shader->setUniformValue( "material.shininess", 10.0f );

    // Draw the cuboids
    m_cube->vertexArrayObject()->bind();
    m_funcs->glDrawElementsInstanced( GL_TRIANGLES, m_cube->indexCount(), GL_UNSIGNED_INT, nullptr, pointCount );

    // Draw the background sphere
    m_sphere->material()->bind();
    shader = m_sphere->material()->shader();

    m_camera->setStandardUniforms( shader, m_sphereModelMatrix );
    shader->setUniformValue( "time", m_time );

    // Let the mesh setup the remainder of its state and draw itself
    m_sphere->render();
}

void InstancedHistogramScene::prepareVertexBuffers()
{
    // Populate the data buffer object
    m_dataBuffer.create();
    m_dataBuffer.setUsagePattern( QOpenGLBuffer::DynamicDraw );
    m_dataBuffer.bind();
    m_dataBuffer.allocate( m_data.data(), m_data.size() * sizeof( float ) );
}

void InstancedHistogramScene::prepareVertexArrayObject()
{
    // Bind the marker's VAO
    m_cube->vertexArrayObject()->bind();

    // Enable the data buffer and add it to the marker's VAO
    QOpenGLShaderProgramPtr shader = m_cube->material()->shader();
    shader->bind();
    m_dataBuffer.bind();
    shader->enableAttributeArray( "point" );
    shader->setAttributeBuffer( "point", GL_FLOAT, 0, 3 );

    // We only vary the point attribute once per instance
    GLuint pointLocation = shader->attributeLocation( "point" );
    m_funcs->glVertexAttribDivisor( pointLocation, 1 );
    m_cube->vertexArrayObject()->release();
}
