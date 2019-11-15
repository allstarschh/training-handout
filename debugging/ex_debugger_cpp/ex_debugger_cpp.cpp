/*************************************************************************
 *
 * Copyright (c) 2016, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <iostream>
#include <cmath>

// when you don't have a pretty printer, you can call helper functions like this one
void print(const std::unordered_map<std::string, std::vector<double>>& complexData)
{
    std::cerr << '{';
    for (const auto& entry : complexData) {
        std::cerr << '{' << entry.first << ", {";
        for (const auto& value : entry.second)
            std::cerr << value << ',';
        std::cerr << "}},";
    }
    std::cerr << "}\n";
}

struct foo
{
    int i;
    float f;
    double d;
};

void printDemo()
{
    // stack variables are easy to access
    int i = 1;

    // but we can also inspect pointers
    double* d = new double(42.);

    // structs and classes are also no problem
    foo f = {2, 3.f, 4.};

    // but try to print the contents of this complex data type
    std::unordered_map<std::string, std::vector<double>> complexData;
    complexData["foo"] = {double(i), double(f.i), f.f, f.d, *d};

    // here you can try `call print(complexData)`

    delete d;
}

double breakPoints()
{
    double s = 0;
    for (int i = 0; i < 10; ++i) {
        double x = std::sin(i);
        double y = std::cos(i);
        // add a breakpoint, only hit it when i is even, then print x and y
        s += x * x + y * y;
    }
    return s;
}

void breakPointsWithFunctionCalls()
{
    // Try printing this variable to verify that your libstdc++ or libc++ pretty printers
    // are properly set up.
    std::vector<std::string> greetings{"Bonjour", "Hallo", "Hello", "Hola"};

    for (size_t i = 0; i < greetings.size(); i++) {
        const std::string &greeting = greetings[i];
        // Add a breakpoint, only hit when greeting equals "Hello", then print i
        std::cerr << greeting << '\n';
    }
}

void infiniteLoop()
{
    std::cerr << "entering infinite loop\n";

    bool run = true;
    while (run) {
        // infinite loop! can you make it quit?
    }

    std::cerr << "great, you exited the loop!\n";
}

void deadlock()
{
    std::mutex mutex;

    // this is not going to work, can you unlock/skip me please?
    std::cerr << "locking mutex\n";
    mutex.lock();

    std::cerr << "locking mutex again\n";
    std::lock_guard<std::mutex> lock(mutex);

    std::cerr << "Yay, you magically unlocked the mutex!\n";
}

int main()
{
    printDemo();

    breakPoints();

    breakPointsWithFunctionCalls();

    infiniteLoop();

    deadlock();

    return 0;
}
