#ifndef FRAMEBUFFEROBJECT_H
#define FRAMEBUFFEROBJECT_H

#include <QtGlobal>
#include <QSharedPointer>
#include <qopengl.h>

class QOpenGLTexture;
class QOpenGLExtension_ARB_framebuffer_object;

class FramebufferObject
{
    Q_DISABLE_COPY( FramebufferObject )

public:
    class AttachmentConfiguration
    {
        Q_DISABLE_COPY( AttachmentConfiguration )

    public:
        virtual ~AttachmentConfiguration();

        GLenum attachmentPoint() const;

    protected:
        // TODO: redefine all the FBO attachment points as a enum

        AttachmentConfiguration( GLenum attachmentPoint = GL_COLOR_ATTACHMENT0 );

        // TODO: add a convenience static returning some list of configuration,
        // f.i. a RGBA color texture and a combined depth/stencil renderbuffer.

    private:
        const GLenum m_attachmentPoint;
    };

    class TextureAttachmentConfiguration : public AttachmentConfiguration
    {
    public:
        // TODO: support attaching "multiple layers" in one go?
        TextureAttachmentConfiguration(const QOpenGLTexture *texture,
                                        GLenum attachmentPoint = GL_COLOR_ATTACHMENT0,
                                        int mipmapLevel = 0,
                                        int layer = 0 );

        const QOpenGLTexture *texture() const;
        int mipmapLevel() const;
        int layer() const;

    private:
        const QOpenGLTexture *m_texture;
        const int m_mipmapLevel;
        const int m_layer;
    };

    class RenderbufferAttachmentConfiguration : public AttachmentConfiguration
    {
    public:
        RenderbufferAttachmentConfiguration( int width,
                                             int height,
                                             GLenum internalFormat,
                                             GLenum attachmentPoint = GL_COLOR_ATTACHMENT0,
                                             int samples = 0 );

        int width() const;
        int height() const;
        GLenum internalFormat() const;
        int samples() const;

    private:
        GLuint m_renderbufferId;
        const int m_width;
        const int m_height;
        const GLenum m_internalFormat;
        const int m_samples;

        friend class FramebufferObject; // for accessing m_renderbufferId
    };

    typedef QSharedPointer< AttachmentConfiguration > AttachmentConfigurationPtr;

    explicit FramebufferObject( const QList< AttachmentConfigurationPtr > &configuration );
    ~FramebufferObject();

    bool create();
    void bind( GLenum target = GL_FRAMEBUFFER );
    void release( GLenum target = GL_FRAMEBUFFER );

    bool isCreated() const;

    QList< AttachmentConfigurationPtr > attachmentsConfiguration() const;
    GLuint fboId() const;

    class FramebufferObjectBinder
    {
        Q_DISABLE_COPY( FramebufferObjectBinder )

    public:
        FramebufferObjectBinder( FramebufferObject *fbo, GLenum target = GL_FRAMEBUFFER );
        ~FramebufferObjectBinder();

    private:
        QOpenGLExtension_ARB_framebuffer_object *m_fboFunctions;
        GLint m_oldReadFBO;
        GLint m_oldDrawFBO;
    };

    // TODO: add OES FBO (sigh)
    // TODO: add "hasFeature" ?
    // TODO: add blit
    // TODO: add empty FBO support (= FBO setters/getters)
    // TODO: destroy on context destroy

private:
    void destroy();

    QList<AttachmentConfigurationPtr> m_configuration;
    QOpenGLExtension_ARB_framebuffer_object *m_fboFunctions;
    GLuint m_fboId;
};

#endif // FRAMEBUFFEROBJECT_H

