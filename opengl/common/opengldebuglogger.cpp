#include "opengldebuglogger.h"

#ifdef HAVE_OPENGL_DEBUG_WINDOW
#include "opengldebugmessagemodel.h"
#include "opengldebugmessagewindow.h"
#endif

#include <QGuiApplication>
#include <QOpenGLDebugLogger>
#include <QDebug>

OpenGLDebugLogger::OpenGLDebugLogger(QObject *parent)
    : QObject(parent)
{
}

OpenGLDebugLogger::~OpenGLDebugLogger()
{
}

void OpenGLDebugLogger::initialize()
{
    const int openglDebugLevel = qEnvironmentVariableIntValue("OPENGL_DEBUG");

    if (openglDebugLevel) {
        QOpenGLDebugLogger *logger = new QOpenGLDebugLogger(this);
        if (logger->initialize()) {
            const QList<QOpenGLDebugMessage> startupMessages = logger->loggedMessages();
            if (qApp && qApp->inherits("QApplication")) {
#if HAVE_OPENGL_DEBUG_WINDOW
                OpenGLDebugMessageModel *model = new OpenGLDebugMessageModel(this);
                model->setMessages(startupMessages);

                connect(logger, &QOpenGLDebugLogger::messageLogged,
                        model, &OpenGLDebugMessageModel::appendMessage);

                m_debugWindow.reset( new OpenGLDebugMessageWindow );
                m_debugWindow->setModel(model);

                qDebug() << "Debug mode activated, press F5 to open the debug log window";
#endif
            } else {
                connect(logger, &QOpenGLDebugLogger::messageLogged,
                        this, &OpenGLDebugLogger::messageLogged);
                qDebug() << "Debug log requested, but QApplication not used -- dumping OpenGL messages via qDebug";

                if (!startupMessages.isEmpty())
                    qDebug() << "Debug messages logged on startup:" << startupMessages;
            }

            if (openglDebugLevel > 1)
                logger->enableMessages(); // also enables "low severity" messages

            logger->startLogging(QOpenGLDebugLogger::SynchronousLogging);
        } else {
            qDebug() << "Debugging requested but logger failed to initialize";
        }
    }
}

void OpenGLDebugLogger::showDebugWindow()
{
#ifdef HAVE_OPENGL_DEBUG_WINDOW
    if ( m_debugWindow )
        m_debugWindow->show();
    else
#endif
        qWarning() << "Cannot show the OpenGL debug window, are you using QApplication?";
}

void OpenGLDebugLogger::messageLogged(const QOpenGLDebugMessage &message)
{
    qDebug() << message;
}
