#ifndef OPENGLSHADERPROGRAM_H
#define OPENGLSHADERPROGRAM_H

#include <QOpenGLShaderProgram>
#include <QSharedPointer>

#include "glslversionhandler.h"

class OpenGLShaderProgram : public QOpenGLShaderProgram
{
    Q_OBJECT
public:
    explicit OpenGLShaderProgram( QObject *parent = nullptr );

    // add the corresponding shader from source code or source file,
    // but since it's unversioned, it first fixes it up by adding the #version
    bool addUnversionedShaderFromSourceCode( QOpenGLShader::ShaderTypeBit type, const QByteArray &source );
    bool addUnversionedShaderFromSourceFile( QOpenGLShader::ShaderTypeBit type, const QString &file );

private:
    GLSLVersionHandler m_versionHandler;
};

typedef QSharedPointer<OpenGLShaderProgram> OpenGLShaderProgramPtr;
typedef QSharedPointer<QOpenGLShaderProgram> QOpenGLShaderProgramPtr;

#endif // OPENGLSHADERPROGRAM_H
