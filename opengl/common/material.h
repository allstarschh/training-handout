#ifndef MATERIAL_H
#define MATERIAL_H

#include "sampler.h"
#include "openglshaderprogram.h"

#include <QMap>
#include <QOpenGLTexture>
#include <QPair>
#include <QSharedPointer>

typedef QSharedPointer<QOpenGLTexture> QOpenGLTexturePtr;

class TextureUnitConfiguration : public QPair<QOpenGLTexturePtr, SamplerPtr>
{
public:
    TextureUnitConfiguration()
        : QPair<QOpenGLTexturePtr, SamplerPtr>( QOpenGLTexturePtr(), SamplerPtr() )
    {
    }

    explicit TextureUnitConfiguration( const QOpenGLTexturePtr& texture, const SamplerPtr& sampler )
        : QPair<QOpenGLTexturePtr, SamplerPtr>( texture, sampler )
    {
    }

    void setTexture( const QOpenGLTexturePtr& texture ) { first = texture; }
    QOpenGLTexturePtr texture() const { return first; }

    void setSampler( const SamplerPtr sampler ) { second = sampler; }
    SamplerPtr sampler() const { return second; }
};

class Material
{
public:
    Material();
    ~Material();

    void bind();
    void release();

    enum LinkBehavior {
        Link,
        DontLink
    };

    void setShaders( const QHash<QOpenGLShader::ShaderTypeBit, QString>& shaders,
                     const LinkBehavior& linkBehavior = Link );

    void setShaders( const QString& vertexShader,
                     const QString& fragmentShader );
#if !defined(QT_OPENGL_ES_2)
    void setShaders( const QString& vertexShader,
                     const QString& geometryShader,
                     const QString& fragmentShader );

    void setShaders( const QString& vertexShader,
                     const QString& tessellationControlShader,
                     const QString& tessellationEvaluationShader,
                     const QString& fragmentShader );

    void setShaders( const QString& vertexShader,
                     const QString& geometryShader,
                     const QString& tessellationControlShader,
                     const QString& tessellationEvaluationShader,
                     const QString& fragmentShader );
#endif

    void setShader(const OpenGLShaderProgramPtr &shader );

    OpenGLShaderProgramPtr shader() const { return m_shader; }

    void setTextureUnitConfiguration( GLuint unit, QOpenGLTexturePtr texture, SamplerPtr sampler );
    void setTextureUnitConfiguration( GLuint unit, QOpenGLTexturePtr texture, SamplerPtr sampler, const QByteArray& uniformName );

    void setTextureUnitConfiguration( GLuint unit, QOpenGLTexturePtr texture );
    void setTextureUnitConfiguration( GLuint unit, QOpenGLTexturePtr texture, const QByteArray& uniformName );

    TextureUnitConfiguration &textureUnitConfiguration( GLuint unit );

private:
    // For now we assume that we own the shader
    /** \todo Allow this to use reference to non-owned shader */
    OpenGLShaderProgramPtr m_shader;

    // This map contains the configuration for the texture units
    QMap<GLuint, TextureUnitConfiguration> m_unitConfigs;
    QMap<GLuint, QByteArray> m_samplerUniforms;
};

typedef QSharedPointer<Material> MaterialPtr;

QOpenGLTexturePtr createSingleLevel2DTexture( int w, int h, QOpenGLTexture::TextureFormat format = QOpenGLTexture::RGBA8_UNorm );

QOpenGLTexturePtr createSRGBTexture( const QImage &image,
                                     QOpenGLTexture::MipMapGeneration genMipMaps );

QOpenGLTexturePtr createTextureFromDDSFile( const QString& filename );
QOpenGLTexturePtr createTextureFromKTXFile( const QString& filename );

MaterialPtr createPostProcessMaterial( const QString& vertexShader,
                                       const QString& fragmentShader,
                                       QOpenGLTexture::TextureFormat format = QOpenGLTexture::RGBA8_UNorm );
MaterialPtr createPostProcessMaterial( const OpenGLShaderProgramPtr &program,
                                       QOpenGLTexture::TextureFormat format = QOpenGLTexture::RGBA8_UNorm );

QVector3D sRgbToLinear( const QVector3D& sRgbColor );

#endif // MATERIAL_H
