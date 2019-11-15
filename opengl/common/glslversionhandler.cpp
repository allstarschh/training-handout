#include "glslversionhandler.h"

#include <QFile>
#include <QOpenGLContext>

GLSLVersionHandler::GLSLVersionHandler( QObject *parent )
    : QObject( parent )
{

}

static QByteArray versionStringForFormat( const QSurfaceFormat &format )
{
    QByteArray versionString = QByteArrayLiteral("#version ");

    const QPair<int, int> contextVersion = format.version();
    if ( format.renderableType() == QSurfaceFormat::OpenGL ) {
        if ( contextVersion >= qMakePair( 3, 2 ) ) {
            if ( contextVersion >= qMakePair( 3, 3 ) )
                versionString += QByteArray::number( contextVersion.first * 100 + contextVersion.second * 10 );
            else
                versionString += QByteArrayLiteral("150"); // 3.2

            versionString += QByteArrayLiteral(" ");

            switch ( format.profile() ) {
            case QSurfaceFormat::CoreProfile:
                versionString += QByteArrayLiteral("core");
                break;
            case QSurfaceFormat::CompatibilityProfile:
                versionString += QByteArrayLiteral("compatibility");
                break;
            default:
                qFatal("Any Desktop GL >= 3.2 must have a profile");
            }
        } else {
            // we'd much like using C++11 here with a constexpr QPair, but we still can't...
            if ( contextVersion == qMakePair( 3, 1 ) )
                versionString += QByteArrayLiteral("140");
            else if ( contextVersion == qMakePair( 3, 0 ) )
                versionString += QByteArrayLiteral("130");
            else if ( contextVersion == qMakePair( 2, 1 ) )
                versionString += QByteArrayLiteral("120");
            else if ( contextVersion == qMakePair( 2, 0 ) )
                versionString += QByteArrayLiteral("110");
            else
                qFatal("The current GL version (%d.%d) does not support shader programs", contextVersion.first, contextVersion.second);
        }
    } else {
        if ( contextVersion >= qMakePair( 3, 0 ) )
            versionString += QByteArray::number( contextVersion.first * 100 + contextVersion.second * 10 );
        else if ( contextVersion == qMakePair( 2, 0 ) )
            versionString += QByteArrayLiteral("100");
        else
            qFatal("The current GL ES version (%d.%d) does not support shader programs", contextVersion.first, contextVersion.second);

        versionString += QByteArrayLiteral(" es");
    }

    versionString += "\n";
    return versionString;
}

QByteArray defaultPrecisionStringForFormat( QOpenGLShader::ShaderTypeBit shaderType, const QSurfaceFormat &format )
{
    // TODO: also all the sampler datatypes (but sampler2D / samplerCube) and image* datatypes
    // require qualifiers in GLSL 3.00 / 3.10, see pagg. 63-64 of the 3.10 spec
    if ( format.renderableType() == QSurfaceFormat::OpenGLES && shaderType == QOpenGLShader::Fragment )
        return QByteArrayLiteral("precision mediump float;\n");

    return QByteArray();
}

QByteArray GLSLVersionHandler::processShaderFromSourceCode( QOpenGLShader::ShaderTypeBit shaderType, const QByteArray &code )
{
    QOpenGLContext *context = QOpenGLContext::currentContext();
    Q_ASSERT(context);

    const QSurfaceFormat contextFormat = context->format();

    const QByteArray versionString = versionStringForFormat( contextFormat );
    const QByteArray defaultPrecisionString = defaultPrecisionStringForFormat( shaderType, contextFormat );

    // simply prepend the "preamble" to the shader's code
    return versionString + defaultPrecisionString + code;
}

QByteArray GLSLVersionHandler::processShaderFromSourceFile( QOpenGLShader::ShaderTypeBit shaderType, const QString &path )
{
    QFile file( path );
    if ( !file.open( QIODevice::ReadOnly ) ) {
        qWarning() << "Could not open source file" << path << ":" << file.errorString();
        return QByteArray();
    }

    const QByteArray contents = file.readAll();
    file.close();

    return processShaderFromSourceCode( shaderType, contents );
}
