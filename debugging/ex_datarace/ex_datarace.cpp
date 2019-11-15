// tools that detect the issue(s) in this code:
// * Valgrind's helgrind
// * Thread Sanitizer

#include <vector>
#include <thread>
#include <future>
#include <iostream>

//--> slide
void race(int *counter, int increment) { *counter += increment; }

int main()
{
    int counter = 0;
    {
        std::vector<std::future<void>> futures;
        const auto threadsToStart = std::thread::hardware_concurrency();
        for (unsigned i = 0; i < threadsToStart; ++i)
            futures.push_back(std::async(std::launch::async, race, &counter, i));
    }
    std::cout << counter << std::endl;
    return 0;
}
//<-- slide
