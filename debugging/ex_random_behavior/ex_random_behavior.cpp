#include <limits>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>

class Semaphore
{
public:
    explicit Semaphore(int n = 0)
        : m_count(n)
    {}

    void release(int n = 1)
    {
        const std::lock_guard<std::mutex> lock(m_mutex);
        m_count += n;
        m_condition.notify_one();
    }

    void acquire(int n = 1)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_count < n)
            m_condition.wait(lock);
        m_count -= n;
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_condition;
    int m_count;
};

Semaphore waitForThreadStartSemaphore(0);
bool workerThreadHasStarted = false;

// Runs in a worker thread
void expensiveComputations()
{
    std::cout << "In worker thread" << std::endl;

    // Using time as input -> possible source of non determinism
    using namespace std::chrono;
    const auto secs = time_point_cast<seconds>(system_clock::now()).time_since_epoch().count();
    workerThreadHasStarted = (secs % 2 == 0);

    waitForThreadStartSemaphore.release(1);

    // Simulate some more expensive stuff
    std::this_thread::sleep_for(milliseconds(1000));
}

// Called by main (used to increase chances of having a data race)
void simulateSmallInitializationDelay()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(3));
}

int main(int , char**)
{
    std::cout << "Main Thread: starting worker thread" << std::endl;

    // Start worker thread
    std::thread workerThread(&expensiveComputations);

    // Simulate delay in initialization
    simulateSmallInitializationDelay();

    // Wait for the thread to release the semaphore before continuing, otherwise
    // the following assert has a data race -> source of non determinism
    waitForThreadStartSemaphore.acquire(1);

    assert(workerThreadHasStarted);

    std::cout << "Main Thread: worker thread started" << std::endl;

    // Wait for thread to exit
    workerThread.join();

    return 0;
}
