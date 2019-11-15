#include "material.h"

#include <QByteArray>
#include <QFile>
#include <QOpenGLContext>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLPixelTransferOptions>

#include <gli.hpp>

#include <cmath>

Material::Material()
    : m_shader( new OpenGLShaderProgram )
{
}

Material::~Material()
{
    m_shader->release();
}

void Material::bind()
{
    m_shader->bind();
    for ( auto it = m_unitConfigs.constBegin(); it != m_unitConfigs.constEnd(); ++it )
    {
        const GLuint unit = it.key();
        const TextureUnitConfiguration& config = it.value();
        config.texture()->bind( unit );

        // Bind the sampler (if present)
        if ( config.sampler() )
            config.sampler()->bind( unit );

        // Associate with sampler uniform in shader (if we know the name or location)
        if ( m_samplerUniforms.contains( unit ) )
            m_shader->setUniformValue( m_samplerUniforms.value( unit ).constData(), unit );
    }
}

void Material::release()
{
    m_shader->release();
    for ( auto it = m_unitConfigs.constBegin(); it != m_unitConfigs.constEnd(); ++it )
    {
        const GLuint unit = it.key();
        const TextureUnitConfiguration& config = it.value();

        config.texture()->release( unit );

        // Bind the sampler (if present)
        if ( config.sampler() )
            config.sampler()->release( unit );
    }
}

void Material::setShaders( const QHash<QOpenGLShader::ShaderTypeBit, QString>& shaders,
                           const LinkBehavior& linkBehavior )
{
    for ( auto it = shaders.constBegin(); it != shaders.constEnd(); ++it )
    {
        if ( !m_shader->addUnversionedShaderFromSourceFile( it.key(), it.value() ) )
            qCritical() << QObject::tr( "Could not compile shader. Log:" ) << m_shader->log();
    }

    if ( linkBehavior == Link )
    {
        if ( !m_shader->link() )
            qCritical() << QObject::tr( "Could not link shader program. Log:" ) << m_shader->log();
    }
}

void Material::setShaders( const QString& vertexShader,
                           const QString& fragmentShader )
{
    // Create a shader program
    if ( !m_shader->addUnversionedShaderFromSourceFile( QOpenGLShader::Vertex, vertexShader ) )
        qCritical() << QObject::tr( "Could not compile vertex shader. Log:" ) << m_shader->log();

    if ( !m_shader->addUnversionedShaderFromSourceFile( QOpenGLShader::Fragment, fragmentShader ) )
        qCritical() << QObject::tr( "Could not compile fragment shader. Log:" ) << m_shader->log();

    if ( !m_shader->link() )
        qCritical() << QObject::tr( "Could not link shader program. Log:" ) << m_shader->log();
}

#ifndef QT_OPENGL_ES_2
void Material::setShaders( const QString& vertexShader,
                           const QString& geometryShader,
                           const QString& fragmentShader )
{
    // Create a shader program
    if ( !m_shader->addUnversionedShaderFromSourceFile( QOpenGLShader::Vertex, vertexShader ) )
        qCritical() << QObject::tr( "Could not compile vertex shader. Log:" ) << m_shader->log();

    if ( !m_shader->addUnversionedShaderFromSourceFile( QOpenGLShader::Geometry, geometryShader ) )
        qCritical() << QObject::tr( "Could not compile geometry shader. Log:" ) << m_shader->log();

    if ( !m_shader->addUnversionedShaderFromSourceFile( QOpenGLShader::Fragment, fragmentShader ) )
        qCritical() << QObject::tr( "Could not compile fragment shader. Log:" ) << m_shader->log();

    if ( !m_shader->link() )
        qCritical() << QObject::tr( "Could not link shader program. Log:" ) << m_shader->log();
}

void Material::setShaders( const QString& vertexShader,
                           const QString& tessellationControlShader,
                           const QString& tessellationEvaluationShader,
                           const QString& fragmentShader )
{
    // Create a shader program
    if ( !m_shader->addUnversionedShaderFromSourceFile( QOpenGLShader::Vertex, vertexShader ) )
        qCritical() << QObject::tr( "Could not compile vertex shader. Log:" ) << m_shader->log();

    if ( !m_shader->addUnversionedShaderFromSourceFile( QOpenGLShader::TessellationControl, tessellationControlShader ) )
        qCritical() << QObject::tr( "Could not compile tessallation control shader. Log:" ) << m_shader->log();

    if ( !m_shader->addUnversionedShaderFromSourceFile( QOpenGLShader::TessellationEvaluation, tessellationEvaluationShader ) )
        qCritical() << QObject::tr( "Could not compile tessellation evaluation shader. Log:" ) << m_shader->log();

    if ( !m_shader->addUnversionedShaderFromSourceFile( QOpenGLShader::Fragment, fragmentShader ) )
        qCritical() << QObject::tr( "Could not compile fragment shader. Log:" ) << m_shader->log();

    if ( !m_shader->link() )
        qCritical() << QObject::tr( "Could not link shader program. Log:" ) << m_shader->log();
}

void Material::setShaders( const QString& vertexShader,
                           const QString& tessellationControlShader,
                           const QString& tessellationEvaluationShader,
                           const QString& geometryShader,
                           const QString& fragmentShader )
{
    // Create a shader program
    if ( !m_shader->addUnversionedShaderFromSourceFile( QOpenGLShader::Vertex, vertexShader ) )
        qCritical() << QObject::tr( "Could not compile vertex shader. Log:" ) << m_shader->log();

    if ( !m_shader->addUnversionedShaderFromSourceFile( QOpenGLShader::TessellationControl, tessellationControlShader ) )
        qCritical() << QObject::tr( "Could not compile tessallation control shader. Log:" ) << m_shader->log();

    if ( !m_shader->addUnversionedShaderFromSourceFile( QOpenGLShader::TessellationEvaluation, tessellationEvaluationShader ) )
        qCritical() << QObject::tr( "Could not compile tessellation evaluation shader. Log:" ) << m_shader->log();

    if ( !m_shader->addUnversionedShaderFromSourceFile( QOpenGLShader::Geometry, geometryShader ) )
        qCritical() << QObject::tr( "Could not compile geometry shader. Log:" ) << m_shader->log();

    if ( !m_shader->addUnversionedShaderFromSourceFile( QOpenGLShader::Fragment, fragmentShader ) )
        qCritical() << QObject::tr( "Could not compile fragment shader. Log:" ) << m_shader->log();

    if ( !m_shader->link() )
        qCritical() << QObject::tr( "Could not link shader program. Log:" ) << m_shader->log();
}
#endif

void Material::setShader( const OpenGLShaderProgramPtr& shader )
{
    m_shader = shader;
}

void Material::setTextureUnitConfiguration( GLuint unit, QOpenGLTexturePtr texture, SamplerPtr sampler )
{
    TextureUnitConfiguration config( texture, sampler );
    m_unitConfigs.insert( unit, config );
}

void Material::setTextureUnitConfiguration( GLuint unit, QOpenGLTexturePtr texture, SamplerPtr sampler, const QByteArray& uniformName )
{
    setTextureUnitConfiguration( unit, texture, sampler );
    m_samplerUniforms.insert( unit, uniformName );
}

void Material::setTextureUnitConfiguration( GLuint unit, QOpenGLTexturePtr texture )
{
    SamplerPtr sampler;
    setTextureUnitConfiguration( unit, texture, sampler );
}

void Material::setTextureUnitConfiguration( GLuint unit, QOpenGLTexturePtr texture, const QByteArray& uniformName )
{
    SamplerPtr sampler;
    setTextureUnitConfiguration( unit, texture, sampler, uniformName );
}

TextureUnitConfiguration &Material::textureUnitConfiguration( GLuint unit )
{
    return m_unitConfigs[unit];
}

// Some useful helper functions
QOpenGLTexturePtr createSingleLevel2DTexture(int w, int h , QOpenGLTexture::TextureFormat format)
{
    QOpenGLTexturePtr texture( new QOpenGLTexture( QOpenGLTexture::Target2D ) );
    texture->setFormat( format );
    texture->setSize( w, h );
    texture->setWrapMode( QOpenGLTexture::ClampToEdge );
    texture->setMinMagFilters( QOpenGLTexture::Nearest, QOpenGLTexture::Nearest );
    texture->setMipLevelRange( 0, 0 );
    texture->allocateStorage();
    return texture;
}

QOpenGLTexturePtr createSRGBTexture( const QImage &image,
                                     QOpenGLTexture::MipMapGeneration genMipMaps )
{
    QOpenGLTexturePtr texture( new QOpenGLTexture( QOpenGLTexture::Target2D ) );
    texture->setFormat( QOpenGLTexture::SRGB8_Alpha8 );
    texture->setSize( image.width(), image.height() );
    texture->setMipLevels( genMipMaps == QOpenGLTexture::GenerateMipMaps ? texture->maximumMipLevels() : 1 );
    texture->allocateStorage();

    QImage glImage = image.convertToFormat( QImage::Format_RGBA8888 );
    QOpenGLPixelTransferOptions uploadOptions;
    uploadOptions.setAlignment(1);
    texture->setData( 0, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, glImage.constBits(), &uploadOptions );
    return texture;
}

QOpenGLTexturePtr createTextureFromDDSFile( const QString& filename )
{
    // Load data using QFile in case filename is a Qt resource
    QFile file( filename );
    if ( !file.open( QIODevice::ReadOnly ) )
        return QOpenGLTexturePtr();
    QByteArray data = file.readAll();
    gli::texture gliTexture = gli::load( data.constData(), data.size() );

    if ( gliTexture.empty() )
        return QOpenGLTexturePtr();

    gli::gl gl;
    gli::gl::format const format = gl.translate( gliTexture.format() );
    GLenum target = gl.translate( gliTexture.target() );

    QOpenGLTexturePtr texture( new QOpenGLTexture( QOpenGLTexture::Target(target) ) );
    texture->setMipBaseLevel( 0 );
    texture->setMipMaxLevel( static_cast<GLint>(gliTexture.levels() - 1) );
    texture->setSwizzleMask( QOpenGLTexture::SwizzleValue( format.Swizzle[0] ),
                             QOpenGLTexture::SwizzleValue( format.Swizzle[1] ),
                             QOpenGLTexture::SwizzleValue( format.Swizzle[2] ),
                             QOpenGLTexture::SwizzleValue( format.Swizzle[3] ) );

    glm::tvec3<GLsizei> const dimensions( gliTexture.dimensions() );
    switch ( gliTexture.target() )
    {
    case gli::TARGET_1D:
    case gli::TARGET_1D_ARRAY:
        texture->setSize( dimensions.x );
        break;

    case gli::TARGET_2D:
    case gli::TARGET_CUBE:
    case gli::TARGET_2D_ARRAY:
    case gli::TARGET_CUBE_ARRAY:
        texture->setSize( dimensions.x, dimensions.y );
        break;

    case gli::TARGET_3D:
        texture->setSize( dimensions.x, dimensions.y, dimensions.z );
        break;

    default:
        Q_ASSERT(false);
        break;
    }

    texture->setMipLevels( static_cast<int>( gliTexture.levels() ) );
    texture->setAutoMipMapGenerationEnabled( false );
    if ( gliTexture.target() == gli::TARGET_1D_ARRAY ||
         gliTexture.target() == gli::TARGET_2D_ARRAY ||
         gliTexture.target() == gli::TARGET_CUBE_ARRAY )
    {
        texture->setLayers( static_cast<int>( gliTexture.layers() ) );
    }
    texture->setFormat( QOpenGLTexture::TextureFormat( format.Internal ) );
    texture->allocateStorage();

    const int layers = static_cast<int>( gliTexture.layers() );
    const int faces = static_cast<int>( gliTexture.faces() );
    const int levels = static_cast<int>( gliTexture.levels() );
    for ( int layer = 0; layer < layers; ++layer )
    {
        for ( int face = 0; face < faces; ++face )
        {
            for ( int level = 0; level < levels; ++level )
            {
                if ( gli::is_compressed( gliTexture.format() ) )
                {
                    texture->setCompressedData( level,
                                                layer,
                                                static_cast<QOpenGLTexture::CubeMapFace>( GL_TEXTURE_CUBE_MAP_POSITIVE_X + face ),
                                                static_cast<int>( gliTexture.size( level ) ),
                                                gliTexture.data( layer, face, level ) );
                }
                else
                {
                    texture->setData( level,
                                      layer,
                                      static_cast<QOpenGLTexture::CubeMapFace>( GL_TEXTURE_CUBE_MAP_POSITIVE_X + face ),
                                      static_cast<QOpenGLTexture::PixelFormat>( format.External ),
                                      static_cast<QOpenGLTexture::PixelType>( format.Type ),
                                      gliTexture.data( layer, face, level ) );
                }
            }
        }
    }

    return texture;
}

QOpenGLTexturePtr createTextureFromKTXFile( const QString& filename )
{
    return createTextureFromDDSFile( filename );
}

MaterialPtr createPostProcessMaterial( const QString& vertexShader,
                                       const QString& fragmentShader,
                                       QOpenGLTexture::TextureFormat format )
{
    // Create a material and set the shaders
    MaterialPtr material( new Material );
    material->setShaders( vertexShader, fragmentShader );

    // Create a texture to render the color buffer into.
    // We start off with a 512x512 texture. This gets re-allocated in the resize() function
    // along with the depth render buffer
    QOpenGLTexturePtr texture( createSingleLevel2DTexture( 512, 512, format ) );

    // Associate the textures with the first texture unit
    material->setTextureUnitConfiguration( 0, texture, QByteArrayLiteral( "texture0" ) );

    return material;
}

MaterialPtr createPostProcessMaterial( const OpenGLShaderProgramPtr &program,
                                       QOpenGLTexture::TextureFormat format )
{
    // Create a material and set the shaders
    MaterialPtr material( new Material );
    material->setShader( program );

    // Create a texture to render the color buffer into.
    // We start off with a 512x512 texture. This gets re-allocated in the resize() function
    // along with the depth render buffer
    QOpenGLTexturePtr texture( createSingleLevel2DTexture( 512, 512, format ) );

    // Associate the textures with the first texture unit
    material->setTextureUnitConfiguration( 0, texture, QByteArrayLiteral( "texture0" ) );

    return material;
}

QVector3D sRgbToLinear( const QVector3D& sRgbColor )
{
    QVector3D c;
    for ( int i = 0; i < 3; ++i )
    {
        if ( sRgbColor[i] <= 0.04045f )
            c[i] = sRgbColor[i] / 12.92f;
        else
            c[i] = std::pow( ( sRgbColor[i] + 0.055f ) / 1.055f, 2.4f );
    }
    return c;
}
