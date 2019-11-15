#ifndef GLSLVERSIONHANDLER_H
#define GLSLVERSIONHANDLER_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QOpenGLShader>
#include <QSharedPointer>

class GLSLVersionHandler : public QObject
{
public:
    explicit GLSLVersionHandler( QObject *parent = nullptr );

    QByteArray processShaderFromSourceCode( QOpenGLShader::ShaderTypeBit shaderType, const QByteArray &code );
    QByteArray processShaderFromSourceFile( QOpenGLShader::ShaderTypeBit shaderType, const QString &path );
};

typedef QSharedPointer<GLSLVersionHandler> GLSLVersionHandlerPtr;

#endif // GLSLVERSIONHANDLER_H
