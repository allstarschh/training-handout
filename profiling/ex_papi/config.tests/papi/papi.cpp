/*************************************************************************
 *
 * Copyright (c) 2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include <papi.h>

int main()
{
    float a, b, d;
    long_long c;
    int r = PAPI_flops(&a, &b, &c, &d);
    return r;
}
