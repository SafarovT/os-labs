#pragma once
#include <cmath>
#include "ThreadsafeSet.h"

namespace
{
	using namespace std;

	struct Interval
	{
		uint64_t start, end;
	};

	bool IsPrime(uint64_t n)
	{
		if (n < 2)
		{
			return false;
		}
		for (uint64_t i = 2; i * i <= n; ++i)
		{
			if (n % i == 0)
			{
				return false;
			}
		}

		return true;
	}

	void FindPrimes(uint64_t start, uint64_t end, ThreadsafeSet<uint64_t>& primes)
	{
		for (uint64_t num = start; num <= end; ++num)
		{
			if (IsPrime(num))
			{
				primes.Insert(num);
			}
		}
	}

	size_t EstimatePrimesBelow(size_t limit)
	{
		return limit < 2
			? 0
			: static_cast<size_t>((limit / log(limit)) * 1.2);
	}

	vector<Interval> SplitBySqrtSum(uint64_t start, uint64_t end, uint64_t intervalsCount)
	{
		double totalSqrtSum = (2.0 / 3) * (pow(end, 1.5) - pow(start - 1, 1.5));

		double targetSum = totalSqrtSum / intervalsCount;

		vector<Interval> intervals;
		uint64_t currentStart = start;

		for (uint64_t i = 0; i < intervalsCount; ++i)
		{
			double nextBoundSqrt = pow(pow(currentStart, 1.5) + (3.0 / 2) * targetSum, 2.0 / 3);
			uint64_t nextEnd = static_cast<uint64_t>(min(static_cast<double>(end), ceil(nextBoundSqrt)));

			intervals.push_back({ currentStart, nextEnd });

			if (nextEnd >= end)
			{
				break;
			}

			currentStart = nextEnd + 1;
		}

		return intervals;
	}
}

void SingleThreadedPrimeSearch(uint64_t limit, unordered_set<uint64_t>& primes)
{
	for (uint64_t num = 2; num <= limit; ++num)
	{
		if (IsPrime(num))
		{
			primes.insert(num);
		}
	}
}

void MultiThreadedPrimeSearch(uint64_t limit, size_t numThreads, ThreadsafeSet<uint64_t>& primes)
{
	vector<thread> threads;
	uint64_t range = limit / numThreads;
	primes.Reserve(EstimatePrimesBelow(limit));
	auto intervals = SplitBySqrtSum(1, limit, numThreads);
	for (auto& interval : intervals)
	{
		threads.emplace_back(FindPrimes, interval.start, interval.end, ref(primes));
	}
	for (auto& thread : threads)
	{
		thread.join();
	}
}
