#include "framebufferobject.h"

#include <QOpenGLContext>
#include <QOpenGLTexture>
#include <QOpenGLExtensions>
#include <QOpenGLFunctions>

FramebufferObject::AttachmentConfiguration::AttachmentConfiguration( GLenum attachmentPoint )
    : m_attachmentPoint( attachmentPoint )
{
}

FramebufferObject::AttachmentConfiguration::~AttachmentConfiguration()
{
}

GLenum FramebufferObject::AttachmentConfiguration::attachmentPoint() const
{
    return m_attachmentPoint;
}

FramebufferObject::TextureAttachmentConfiguration::TextureAttachmentConfiguration( const QOpenGLTexture *texture,
                                                                                   GLenum attachmentPoint,
                                                                                   int mipmapLevel,
                                                                                   int layer )
    : AttachmentConfiguration( attachmentPoint )
    , m_texture( texture )
    , m_mipmapLevel( mipmapLevel )
    , m_layer( layer )
{
}

const QOpenGLTexture *FramebufferObject::TextureAttachmentConfiguration::texture() const
{
    return m_texture;
}

int FramebufferObject::TextureAttachmentConfiguration::mipmapLevel() const
{
    return m_mipmapLevel;
}

int FramebufferObject::TextureAttachmentConfiguration::layer() const
{
    return m_layer;
}

FramebufferObject::RenderbufferAttachmentConfiguration::RenderbufferAttachmentConfiguration( int width,
                                                                                             int height,
                                                                                             GLenum internalFormat,
                                                                                             GLenum attachmentPoint,
                                                                                             int samples )
    : AttachmentConfiguration( attachmentPoint )
    , m_renderbufferId( 0 )
    , m_width( width )
    , m_height( height )
    , m_internalFormat( internalFormat )
    , m_samples( samples )
{
}

int FramebufferObject::RenderbufferAttachmentConfiguration::width() const
{
    return m_width;
}

int FramebufferObject::RenderbufferAttachmentConfiguration::height() const
{
    return m_height;
}

GLenum FramebufferObject::RenderbufferAttachmentConfiguration::internalFormat() const
{
    return m_internalFormat;
}

int FramebufferObject::RenderbufferAttachmentConfiguration::samples() const
{
    return m_samples;
}


FramebufferObject::FramebufferObject(const QList< FramebufferObject::AttachmentConfigurationPtr > &configuration)
    : m_configuration( configuration )
    , m_fboFunctions( new QOpenGLExtension_ARB_framebuffer_object )
    , m_fboId( 0 )
{
}

FramebufferObject::~FramebufferObject()
{
    destroy();
    delete m_fboFunctions;
}

bool FramebufferObject::create()
{
    Q_ASSERT( QOpenGLContext::currentContext() );

    // already created?
    if ( isCreated() )
        return true;

    if ( !m_fboFunctions->initializeOpenGLFunctions() )
        return false;

    QOpenGLFunctions *functions = QOpenGLContext::currentContext()->functions();

    m_fboFunctions->glGenFramebuffers( 1, &m_fboId );

    FramebufferObjectBinder binder( this );

    bool successfulCreation = true;

    foreach ( AttachmentConfigurationPtr attachmentConfiguration, m_configuration ) {
        AttachmentConfiguration *attachmentConfigurationPtr = attachmentConfiguration.data();

        if ( TextureAttachmentConfiguration *conf =
             dynamic_cast< TextureAttachmentConfiguration * >( attachmentConfigurationPtr ) ) {
            const QOpenGLTexture *texture = conf->texture();
            Q_ASSERT( texture );
            Q_ASSERT( texture->isCreated() );

            const QOpenGLTexture::Target textureTarget = texture->target();
            const GLuint textureId = texture->textureId();

            switch ( textureTarget ) {
            case QOpenGLTexture::Target1D:
                m_fboFunctions->glFramebufferTexture1D( GL_FRAMEBUFFER,
                                                        conf->attachmentPoint(),
                                                        textureTarget,
                                                        textureId,
                                                        conf->mipmapLevel() );
                break;

            case QOpenGLTexture::Target2D:
            case QOpenGLTexture::Target2DMultisample:
            case QOpenGLTexture::TargetRectangle:
                m_fboFunctions->glFramebufferTexture2D( GL_FRAMEBUFFER,
                                                        conf->attachmentPoint(),
                                                        textureTarget,
                                                        textureId,
                                                        conf->mipmapLevel() );
                break;

            case QOpenGLTexture::Target1DArray:
            case QOpenGLTexture::Target2DArray:
            case QOpenGLTexture::Target2DMultisampleArray:
            case QOpenGLTexture::Target3D:
            case QOpenGLTexture::TargetCubeMap:
            case QOpenGLTexture::TargetCubeMapArray:
                m_fboFunctions->glFramebufferTextureLayer( GL_FRAMEBUFFER,
                                                           conf->attachmentPoint(),
                                                           textureId,
                                                           conf->mipmapLevel(),
                                                           conf->layer() );
                break;

            case QOpenGLTexture::TargetBuffer:
                qWarning( "Cannot attach texture buffers to a framebuffer object" );
                successfulCreation = false;
                break;
            }

        } else if ( RenderbufferAttachmentConfiguration *conf =
                    dynamic_cast< RenderbufferAttachmentConfiguration * >( attachmentConfigurationPtr ) ) {

            GLint boundRenderbuffer;
            functions->glGetIntegerv( GL_RENDERBUFFER_BINDING , &boundRenderbuffer );

            m_fboFunctions->glGenRenderbuffers( 1, &conf->m_renderbufferId );
            m_fboFunctions->glBindRenderbuffer( GL_RENDERBUFFER, conf->m_renderbufferId );

            // could unify with one call to the multisampled storage,
            // but I fear bugs in broken drivers...
            if ( conf->samples() == 0 ) {
                m_fboFunctions->glRenderbufferStorage( GL_RENDERBUFFER,
                                                       conf->internalFormat(),
                                                       conf->width(),
                                                       conf->height() );
            } else {
                m_fboFunctions->glRenderbufferStorageMultisample( GL_RENDERBUFFER,
                                                                  conf->samples(),
                                                                  conf->internalFormat(),
                                                                  conf->width(),
                                                                  conf->height() );
            }

            m_fboFunctions->glFramebufferRenderbuffer( GL_FRAMEBUFFER,
                                                       conf->attachmentPoint(),
                                                       GL_RENDERBUFFER,
                                                       conf->m_renderbufferId );

            m_fboFunctions->glBindRenderbuffer( GL_RENDERBUFFER, boundRenderbuffer );
        } else {
            qFatal( "Unexpected attachment configuration type" );
        }

        if ( !successfulCreation )
            break;
    }

    if ( successfulCreation ) {
        GLenum fboStatus = m_fboFunctions->glCheckFramebufferStatus( GL_FRAMEBUFFER );
        if ( fboStatus != GL_FRAMEBUFFER_COMPLETE ) {
            qWarning( "Framebuffer incomplete -- check returned %x", fboStatus );
            successfulCreation = false;
        }
    }

    if ( !successfulCreation )
        destroy();

    return successfulCreation;
}

void FramebufferObject::bind( GLenum target )
{
    Q_ASSERT( isCreated() );
    m_fboFunctions->glBindFramebuffer( target, m_fboId );
}

void FramebufferObject::release( GLenum target )
{
    Q_ASSERT( isCreated() );
    m_fboFunctions->glBindFramebuffer( target, 0 );
}

bool FramebufferObject::isCreated() const
{
    return m_fboId != 0;
}

QList< FramebufferObject::AttachmentConfigurationPtr > FramebufferObject::attachmentsConfiguration() const
{
    return m_configuration;
}

GLuint FramebufferObject::fboId() const
{
    return m_fboId;
}

void FramebufferObject::destroy()
{
    foreach ( AttachmentConfigurationPtr attachmentConfiguration, m_configuration ) {
        if ( RenderbufferAttachmentConfiguration *conf =
             dynamic_cast< RenderbufferAttachmentConfiguration * >( attachmentConfiguration.data() ) ) {
            m_fboFunctions->glDeleteRenderbuffers( 1, &conf->m_renderbufferId );
            conf->m_renderbufferId = 0;
        }
    }

    m_fboFunctions->glDeleteFramebuffers( 1, &m_fboId );
    m_fboId = 0;
}


FramebufferObject::FramebufferObjectBinder::FramebufferObjectBinder( FramebufferObject *fbo, GLenum target )
    : m_fboFunctions( new QOpenGLExtension_ARB_framebuffer_object )
{
    Q_ASSERT( fbo );
    Q_ASSERT( fbo->isCreated() );
    Q_ASSERT( QOpenGLContext::currentContext() );

    bool openGLFunctionsInitialized = m_fboFunctions->initializeOpenGLFunctions();
    Q_ASSERT( openGLFunctionsInitialized );
    Q_UNUSED( openGLFunctionsInitialized );

    QOpenGLFunctions *functions = QOpenGLContext::currentContext()->functions();

    functions->glGetIntegerv( GL_READ_FRAMEBUFFER_BINDING, &m_oldReadFBO );
    functions->glGetIntegerv( GL_DRAW_FRAMEBUFFER_BINDING, &m_oldDrawFBO );

    m_fboFunctions->glBindFramebuffer( target, fbo->fboId() );
}

FramebufferObject::FramebufferObjectBinder::~FramebufferObjectBinder()
{
    m_fboFunctions->glBindFramebuffer( GL_READ_FRAMEBUFFER, m_oldReadFBO );
    m_fboFunctions->glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_oldDrawFBO );

    delete m_fboFunctions;
}
