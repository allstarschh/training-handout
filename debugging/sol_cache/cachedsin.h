/*************************************************************************
 *
 * Copyright (c) 2016, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#ifndef CACHEDSIN_H
#define CACHEDSIN_H

#include <vector>

/**
 * Approximates sin with a cache of precomputed values over one full period.
 */
class CachedSin
{
public:
    /**
     * Splits the full 2 pi period into equidistant @p numValues points and
     * calculates sin for each of these points. The results are stored in a
     * cache that will then be used by @c operator()(float).
     */
    explicit CachedSin(int numValues);
    ~CachedSin();

    /**
     * @return approximated sin(x)
     */
    float operator()(float x) const;

private:
    // cached values
    float *m_values;
    int m_numValues;
    // distance between cached values, i.e. 2 pi / m_numValues
    float m_deltaX;
};

#endif
