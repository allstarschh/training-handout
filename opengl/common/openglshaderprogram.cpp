#include "openglshaderprogram.h"


OpenGLShaderProgram::OpenGLShaderProgram( QObject *parent )
    : QOpenGLShaderProgram( parent )
{

}

bool OpenGLShaderProgram::addUnversionedShaderFromSourceCode( QOpenGLShader::ShaderTypeBit type, const QByteArray &source )
{
    return addShaderFromSourceCode( type, m_versionHandler.processShaderFromSourceCode( type, source ) );
}

bool OpenGLShaderProgram::addUnversionedShaderFromSourceFile( QOpenGLShader::ShaderTypeBit type, const QString &file )
{
    return addShaderFromSourceCode( type, m_versionHandler.processShaderFromSourceFile( type, file ) );
}
