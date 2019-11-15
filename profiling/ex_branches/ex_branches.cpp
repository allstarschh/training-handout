/*************************************************************************
 *
 * Copyright (c) 2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include <algorithm>
#include <iostream>
#include <qglobal.h>

// example code based on:
// http://stackoverflow.com/questions/11227809/why-is-it-faster-to-process-a-sorted-array-than-an-unsorted-array
int main(int argc, char** /*argv*/)
{
    // generate random input data
    const unsigned arraySize = 32768;
    int data[arraySize];

    for (unsigned c = 0; c < arraySize; ++c)
        data[c] = std::rand() % 256;

    // optionally sort when any CLI argument is given
    if (argc > 1)
        std::sort(data, data + arraySize);


    // actually run the benchmark
    long long sum = 0;
    for (unsigned i = 0; i < 50000; ++i) {
        for (unsigned c = 0; c < arraySize; ++c) {
            // NOTE: also experiment with Q_LIKELY / Q_UNLIKELY here with a threshold of 192
            if ((data[(c + i) % arraySize] >= 128))
                sum += data[c];
        }
    }

    // output to value to prevent the compiler from optimizing it out
    std::cout << sum << std::endl;
    return 0;
}
