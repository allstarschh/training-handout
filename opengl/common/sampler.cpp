#include "sampler.h"

#include <QDebug>
#include <QOpenGLContext>

class SamplerPrivate
{
public:
    SamplerPrivate( Sampler* qq )
        : q_ptr( qq ),
          m_samplerId( 0 )
    {
    }

    bool create( QOpenGLContext* ctx );
    void destroy();
    void bind( GLuint unit );
    void release( GLuint unit );
    void setParameter( GLenum param, GLenum value );
    void setParameter( GLenum param, float value );

    Q_DECLARE_PUBLIC( Sampler )

    Sampler* q_ptr;
    GLuint m_samplerId;

    struct SamplerFunctions {
        typedef void (QOPENGLF_APIENTRYP qt_glGenSamplers_t)(GLsizei n, GLuint *samplers);
        typedef void (QOPENGLF_APIENTRYP qt_glDeleteSamplers_t)(GLsizei n, const GLuint *samplers);
        typedef void (QOPENGLF_APIENTRYP qt_glBindSampler_t)(GLuint unit, GLuint sampler);
        typedef void (QOPENGLF_APIENTRYP qt_glSamplerParameteri_t)(GLuint sampler, GLenum pname, GLint param);
        typedef void (QOPENGLF_APIENTRYP qt_glSamplerParameterf_t)(GLuint sampler, GLenum pname, GLfloat param);

        qt_glGenSamplers_t glGenSamplers;
        qt_glDeleteSamplers_t glDeleteSamplers;
        qt_glBindSampler_t glBindSampler;
        qt_glSamplerParameteri_t glSamplerParameteri;
        qt_glSamplerParameterf_t glSamplerParameterf;

        bool resolve( QOpenGLContext *ctx )
        {
#ifdef QT_OPENGL_ES_3
            glGenSamplers = &::glGenSamplers;
            glDeleteSamplers = &::glDeleteSamplers;
            glBindSampler = &::glBindSampler;
            glSamplerParameteri = &::glSamplerParameteri;
            glSamplerParameterf = &::glSamplerParameterf;

            return true;
#else
#define GET_SAMPLER_PROC_ADDRESS(procName) \
    procName = reinterpret_cast< qt_ ## procName ## _t >( ctx->getProcAddress( QByteArrayLiteral( #procName ) ) )

            GET_SAMPLER_PROC_ADDRESS( glGenSamplers );
            GET_SAMPLER_PROC_ADDRESS( glDeleteSamplers );
            GET_SAMPLER_PROC_ADDRESS( glBindSampler );
            GET_SAMPLER_PROC_ADDRESS( glSamplerParameteri );
            GET_SAMPLER_PROC_ADDRESS( glSamplerParameterf );

#undef GET_SAMPLER_PROC_ADDRESS

            return glGenSamplers
                    && glDeleteSamplers
                    && glBindSampler
                    && glSamplerParameteri
                    && glSamplerParameterf;
#endif // QT_OPENGL_ES_3
        }
    } m_samplerFunctions;
};

bool SamplerPrivate::create( QOpenGLContext* ctx )
{
    if ( ctx->isOpenGLES() ) {
        if ( ctx->format().version() >= qMakePair( 3, 0 ) ) {
            bool ok = m_samplerFunctions.resolve( ctx );
            if (!ok)
                qWarning() << "Failed to resolve sampler functions";
            Q_ASSERT( ok );
        } else {
            qWarning("The OpenGL ES context version does not support sampler objects");
            return false;
        }
    } else {
        if ( ctx->hasExtension( "GL_ARB_sampler_objects" ) ) {
            bool ok = m_samplerFunctions.resolve( ctx );
            if (!ok)
                qWarning() << "Failed to resolve sampler functions";
            Q_ASSERT( ok );
        } else {
            qWarning("The OpenGL context version does not support sampler objects");
            return false;
        }
    }

    Q_ASSERT( m_samplerFunctions.glGenSamplers );
    m_samplerFunctions.glGenSamplers( 1, &m_samplerId );
    return ( m_samplerId != 0 );
}

void SamplerPrivate::destroy()
{
    if ( m_samplerId ) {
        m_samplerFunctions.glDeleteSamplers( 1, &m_samplerId );
        m_samplerId = 0;
    }
}

void SamplerPrivate::bind( GLuint unit )
{
    if ( m_samplerId )
        m_samplerFunctions.glBindSampler( unit, m_samplerId );
}

void SamplerPrivate::release( GLuint unit )
{
    m_samplerFunctions.glBindSampler( unit, 0 );
}

void SamplerPrivate::setParameter( GLenum param, GLenum value )
{
    if ( m_samplerId )
        m_samplerFunctions.glSamplerParameteri( m_samplerId, param, value );
}

void SamplerPrivate::setParameter( GLenum param, float value )
{
    if ( m_samplerId )
        m_samplerFunctions.glSamplerParameterf( m_samplerId, param, value );
}

Sampler::Sampler()
    : d_ptr( new SamplerPrivate( this ) )
{
}

Sampler::~Sampler()
{
}

bool Sampler::create()
{
    QOpenGLContext* context = QOpenGLContext::currentContext();
    Q_ASSERT( context );
    Q_D( Sampler );
    return d->create( context );
}

void Sampler::destroy()
{
    Q_D( Sampler );
    d->destroy();
}

bool Sampler::isCreated() const
{
    Q_D( const Sampler );
    return ( d->m_samplerId != 0 );
}

GLuint Sampler::samplerId() const
{
    Q_D( const Sampler );
    return d->m_samplerId;
}

void Sampler::bind( GLuint unit )
{
    Q_D( Sampler );
    d->bind( unit );
}

void Sampler::release( GLuint unit )
{
    Q_D( Sampler );
    d->release( unit );
}

void Sampler::setWrapMode( CoordinateDirection direction, GLenum wrapMode )
{
    Q_D( Sampler );
    d->setParameter( direction, wrapMode );
}

void Sampler::setMinificationFilter( GLenum filter )
{
    Q_D( Sampler );
    d->setParameter( GL_TEXTURE_MIN_FILTER, filter );
}

void Sampler::setMagnificationFilter( GLenum filter )
{
    Q_D( Sampler );
    d->setParameter( GL_TEXTURE_MAG_FILTER, filter );
}

void Sampler::setMaximumAnisotropy( float anisotropy )
{
    Q_D( Sampler );
    d->setParameter( GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy );
}
