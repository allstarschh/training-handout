#include "cachedsin.h"

#include <cmath>

CachedSin::CachedSin(int numValues)
    : m_values(new float[numValues])
    , m_numValues(numValues)
    , m_deltaX(2.f * M_PI / numValues)
{
    float x = 0;
    for (int i = 0; i < m_numValues; ++i, x += m_deltaX) {
        m_values[i] = std::sin(x);
    }
}

CachedSin::~CachedSin()
{
    delete[] m_values;
}

float CachedSin::operator()(float x) const
{
    int index = static_cast<int>(x / m_deltaX);
    return m_values[index];
}
