#include "openglframetimer.h"

OpenGLFrameTimer::OpenGLFrameTimer()
    : m_frameTimers( 2 ),
      m_sampleCount( 2 ),
      m_samplesRecorded( 0 ),
      m_currentFrameTimer( 0 ),
      m_nextPendingFrameTimer( 0 ),
      m_currentMonitor( nullptr )
{
}

void OpenGLFrameTimer::setLabels(const QStringList &labels)
{
    m_labels = labels;
}

QStringList OpenGLFrameTimer::labels() const
{
    return m_labels;
}

void OpenGLFrameTimer::setSampleCount(int sampleCount)
{
    m_sampleCount = sampleCount;
}

int OpenGLFrameTimer::sampleCount() const
{
    return m_sampleCount;
}

void OpenGLFrameTimer::recordSample()
{
    // Is this the first sample of a frame?
    if (m_samplesRecorded == 0) {
        // Obtain a free FrameTimer
        m_currentMonitor = createOrGetTimeMonitor();
    }

    Q_CHECK_PTR(m_currentMonitor);
    m_currentMonitor->recordSample();
    ++m_samplesRecorded;

    // Is this the last sample for this frame?
    if (m_samplesRecorded == m_currentMonitor->sampleCount()) {
        // Move to the next frame
        moveToNextFrame();
        m_currentMonitor = nullptr;
    }
}

bool OpenGLFrameTimer::isResultReady()
{
    return m_frameTimers.at( m_nextPendingFrameTimer ).m_queriesIssued &&
           m_frameTimers.at( m_nextPendingFrameTimer ).m_timeMonitor->isResultAvailable();
}

QVector<GLuint64> OpenGLFrameTimer::waitForIntervals()
{
    // Get the results of our oldest pending set of queries
    QOpenGLTimeMonitor* resultsMonitor = m_frameTimers[m_nextPendingFrameTimer].m_timeMonitor;
    QVector<GLuint64> intervals = resultsMonitor->waitForIntervals();

    // Clear the query results ready for the next time we need to use this monitor
    resultsMonitor->reset();

    // Move to the next set of pending results
    m_frameTimers[m_nextPendingFrameTimer].m_queriesIssued = false;
    int tmp = ++m_nextPendingFrameTimer % m_frameTimers.size();
    m_nextPendingFrameTimer = tmp;

    return intervals;
}

int OpenGLFrameTimer::frameLag() const
{
    int lag = 0;
    if ( m_currentFrameTimer > m_nextPendingFrameTimer )
        lag = m_currentFrameTimer - m_nextPendingFrameTimer;
    else
        lag = m_currentFrameTimer - m_nextPendingFrameTimer + m_frameTimers.size();
    return lag;
}

void OpenGLFrameTimer::moveToNextFrame()
{
    // Move to the next frame timer in a round-robin fashion
    m_frameTimers[m_currentFrameTimer].m_queriesIssued = true;
    int tmp = ++m_currentFrameTimer % m_frameTimers.size();
    m_currentFrameTimer = tmp;
    m_samplesRecorded = 0;
}

QOpenGLTimeMonitor* OpenGLFrameTimer::createOrGetTimeMonitor()
{
    // Find next available frame timer, extending vector of frame timers if needed
    if ( m_frameTimers.at( m_currentFrameTimer ).m_queriesIssued )
    {
        // GL is lagging too far behind, insert another frame timer
        FrameTimer additionalFrameTimer;
        additionalFrameTimer.m_timeMonitor->setSampleCount( m_sampleCount );
        additionalFrameTimer.m_timeMonitor->create();
        m_frameTimers.insert( m_currentFrameTimer, additionalFrameTimer );
        ++m_nextPendingFrameTimer;
    }

    QOpenGLTimeMonitor *timeMonitor = m_frameTimers.at(m_currentFrameTimer).m_timeMonitor;
    if (timeMonitor->sampleCount() != m_sampleCount) {
        timeMonitor->destroy();
        timeMonitor->setSampleCount(m_sampleCount);
        timeMonitor->create();
    }
    return timeMonitor;
}
