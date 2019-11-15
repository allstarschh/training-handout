/*************************************************************************
 *
 * Copyright (c) 2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include <cmath>
#include <random>
#include <iostream>

int main()
{
//--> slide
    std::uniform_real_distribution<double> uniform(-1E5, 1E5);
    std::default_random_engine engine;
    double s = 0;
    for (int i = 0; i < 100000000; ++i) {
        s += uniform(engine);
    }
//<-- slide
    std::cout << "random sum: " << s << '\n';
    return 0;
}
