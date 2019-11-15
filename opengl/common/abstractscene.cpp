#include "abstractscene.h"

#include <QMutexLocker>

#include <cmath>

AbstractScene::AbstractScene( QObject* parent )
    : QObject( parent )
    , m_mutex( QMutex::Recursive )
    , m_paused( false )
    , m_modelTheta( 0.0f )
    , m_lastTime( 0.0f )
    , m_thetaDegreesPerSecond( 60.0f )

{
}

AbstractScene::~AbstractScene()
{
}

void AbstractScene::update(float t)
{
    QMutexLocker locker(mutex());

    if ( !m_paused ) {
        float dt = t - m_lastTime;
        m_modelTheta += dt * m_thetaDegreesPerSecond;
        m_modelTheta = std::fmod( m_modelTheta, 360.0f );
    }
    m_lastTime = t;
}

void AbstractScene::togglePause()
{
    QMutexLocker locker(mutex());

    m_paused = !m_paused;
}

void AbstractScene::adjustModelRotationRate(float offset)
{
    QMutexLocker locker(mutex());

    m_thetaDegreesPerSecond += offset;
}

float AbstractScene::modelTheta() const
{
    QMutexLocker locker(mutex());

    return m_modelTheta;
}
