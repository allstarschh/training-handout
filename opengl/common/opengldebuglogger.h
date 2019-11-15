#ifndef OPENGLDEBUGLOGGER_H
#define OPENGLDEBUGLOGGER_H

#include <QObject>
#include <QScopedPointer>

class OpenGLDebugMessageWindow;

class QOpenGLDebugMessage;

class OpenGLDebugLogger : public QObject
{
    Q_OBJECT
public:
    explicit OpenGLDebugLogger(QObject *parent = nullptr);
    ~OpenGLDebugLogger();

    void initialize();

public slots:
    void showDebugWindow();

private slots:
    void messageLogged(const QOpenGLDebugMessage &message);

private:
#ifdef HAVE_OPENGL_DEBUG_WINDOW
    QScopedPointer<OpenGLDebugMessageWindow> m_debugWindow;
#endif
};

#endif // OPENGLDEBUGLOGGER_H
