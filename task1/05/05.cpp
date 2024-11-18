#include <iostream>
#include <ctime>
#include "utils.h"

namespace
{
    using namespace std;

    const size_t MAX_THREADS_COUNT = 16;

    void PrintTimeOfAlgo(uint64_t limit, size_t threadsNumber)
    {
        ThreadsafeSet<uint64_t> primes;
        auto start = chrono::steady_clock::now();
        MultiThreadedPrimeSearch(limit, threadsNumber, primes);
        auto end = chrono::steady_clock::now();
        chrono::duration<double> duration = end - start;
        cout << "Time for " << threadsNumber << " threads = " << duration.count() << " seconds" << endl;
    }

    void PrintTimeForLimitWithDifThreadsNumber(uint64_t limit)
    {
        cout << "Limit = " << limit << endl;
        for (auto threadsCount = 1; threadsCount <= MAX_THREADS_COUNT; threadsCount++)
        {
            PrintTimeOfAlgo(limit, threadsCount);
        }
    }
}

int main()
{
    uint64_t limit = 1000000;
    size_t threadsNumber = 4;

    unordered_set<uint64_t> singleThreadedPrimes;
    auto start = chrono::steady_clock::now();
    SingleThreadedPrimeSearch(limit, singleThreadedPrimes);
    auto end = chrono::steady_clock::now();
    chrono::duration<double> singleThreadedDuration = end - start;
    cout << "Single threaded time = " << singleThreadedDuration.count() << " seconds" << endl;

    ThreadsafeSet<uint64_t> multiThreadedPrimes;
    start = chrono::steady_clock::now();
    MultiThreadedPrimeSearch(limit, threadsNumber, multiThreadedPrimes);
    end = chrono::steady_clock::now();
    chrono::duration<double> multiThreadedDuration = end - start;
    cout << "Multi threaded time (" << threadsNumber<< " threads) = " << multiThreadedDuration.count() << " seconds" << endl;

    cout << "Prime numbers (single thread): ";
    size_t count = 0;
    for (auto prime : singleThreadedPrimes)
    {
        if (count++ >= 10)
        {
            break;
        }
        cout << prime << " ";
    }
    cout << endl << "Prime numbers (multi threads): ";
    for (auto prime : multiThreadedPrimes.GetSomeElements(10))
    {
        cout << prime << " ";
    }
    cout << endl << "-----------------------------" << endl;

    cout << "Time results:" << endl;
    PrintTimeForLimitWithDifThreadsNumber(1000000); // 1.000.000
    PrintTimeForLimitWithDifThreadsNumber(10000000); // 10.000.000
    PrintTimeForLimitWithDifThreadsNumber(100000000); // 100.000.000

    return EXIT_SUCCESS;
}
