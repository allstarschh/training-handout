/*************************************************************************
 *
 * Copyright (c) 2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include <cmath>
#include <iostream>
#include <thread>
#include <future>

struct Results
{
    virtual ~Results() = default;
    virtual unsigned int* data1() = 0;
    virtual unsigned int* data2() = 0;
    virtual unsigned int* data3() = 0;
    virtual unsigned int* data4() = 0;
};

// the two data elements are distinct but share a single cache line
struct Unaligned : Results
{
    unsigned int* data1() override
    {
        return &m_data1;
    }
    unsigned int* data2() override
    {
        return &m_data2;
    }
    unsigned int* data3() override
    {
        return &m_data3;
    }
    unsigned int* data4() override
    {
        return &m_data4;
    }
    unsigned int m_data1 = 0;
    unsigned int m_data2 = 0;
    unsigned int m_data3 = 0;
    unsigned int m_data4 = 0;
};

// the two data elements are distinct and live on separate cache lines
struct Aligned : Results
{
    unsigned int* data1() override
    {
        return &m_data1;
    }
    unsigned int* data2() override
    {
        return &m_data2;
    }
    unsigned int* data3() override
    {
        return &m_data3;
    }
    unsigned int* data4() override
    {
        return &m_data4;
    }
    unsigned int m_data1 = 0;
    alignas(64) unsigned int m_data2 = 0;
    alignas(64) unsigned int m_data3 = 0;
    alignas(64) unsigned int m_data4 = 0;
};

void do_something(volatile unsigned int* result)
{
    for (size_t i = 0; i < 500000000; ++i) {
        *result += sqrt(i);
    }
}

int main(int argc, char** /*argv*/)
{
    // when any command line argument is given, use the aligned data, otherwise
    // use the unaligned data to show the effect of false sharing
    std::unique_ptr<Results> results(argc > 1
        ? static_cast<Results*>(new Aligned)
        : static_cast<Results*>(new Unaligned));
    {
        const auto f1 = std::async(std::launch::async, do_something,
                                   results->data1());
        const auto f2 = std::async(std::launch::async, do_something,
                                   results->data2());
        const auto f3 = std::async(std::launch::async, do_something,
                                   results->data3());
        const auto f4 = std::async(std::launch::async, do_something,
                                   results->data4());
    }

    std::cout << "random sums: " << *results->data1()
              << ", " << *results->data2()
              << ", " << *results->data3()
              << ", " << *results->data4() << '\n';
    return 0;
}
