#ifndef MATHUTILS_H
#define MATHUTILS_H

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <math.h>

double bessel_j0( double x )
{
#if defined(Q_OS_WIN)
    return _j0( x );
#else
    return j0( x );
#endif
}

#endif // MATHUTILS_H
