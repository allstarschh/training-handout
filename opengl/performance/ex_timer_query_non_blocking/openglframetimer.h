#ifndef OPENGLFRAMETIMER_H
#define OPENGLFRAMETIMER_H

#include <qopengl.h>
#include <QOpenGLTimeMonitor>
#include <QStringList>
#include <QVector>

class OpenGLFrameTimer
{
public:
    OpenGLFrameTimer();

    void setLabels(const QStringList &labels);
    QStringList labels() const;

    void setSampleCount(int sampleCount);
    int sampleCount() const;

    void recordSample();

    bool isResultReady();
    QVector<GLuint64> waitForIntervals();

    int frameLag() const;

private:
    QOpenGLTimeMonitor *createOrGetTimeMonitor();
    void moveToNextFrame();

    struct FrameTimer
    {
        FrameTimer()
            : m_timeMonitor( new QOpenGLTimeMonitor ),
              m_queriesIssued( false )
        {}

        QOpenGLTimeMonitor* m_timeMonitor;
        bool m_queriesIssued;
    };

    QVector<FrameTimer> m_frameTimers;
    int m_sampleCount;
    int m_samplesRecorded;

    int m_currentFrameTimer; // Most recently issued set of queries
    int m_nextPendingFrameTimer; // The next set of results we are expecting
    QOpenGLTimeMonitor *m_currentMonitor;

    QStringList m_labels;
};

#endif // OPENGLFRAMETIMER_H
