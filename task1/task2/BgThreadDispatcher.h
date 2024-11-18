#pragma once
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <iostream>

class BgThreadDispatcher
{
public:
	using Task = std::function<void()>;

	BgThreadDispatcher()
		: m_stopFlag(false)
	{
		m_workerThread = std::jthread([&](std::stop_token stopToken) { Run(stopToken); });
	}

	~BgThreadDispatcher()
	{
		// нужен ли здесть stop
		Stop();
	}

	void Dispatch(Task task)
	{
		if (m_stopFlag.load())
		{
			return;
		}
		
		std::lock_guard<std::mutex> lock(m_queueMutex);
		m_taskQueue.push(std::move(task));
		m_queueCondition.notify_one();
	}

	void Wait()
	{
		std::unique_lock<std::mutex> lock(m_queueMutex);
		m_finishedCondition.wait(lock, [&] { return m_taskQueue.empty() && m_isCompleted; });
	}

	void Stop()
	{
		m_stopFlag.store(true);
		m_queueCondition. ();
		if (m_workerThread.joinable())
		{
			m_workerThread.join();
		}
	}

private:
	std::jthread m_workerThread;
	std::queue<Task> m_taskQueue;
	std::mutex m_queueMutex;
	// condition переименовать
	std::condition_variable m_queueCondition;
	std::condition_variable m_finishedCondition;
	std::atomic<bool> m_stopFlag;
	bool m_isCompleted = true;

	bool WaitNewTaskOrStop(std::stop_token stopToken, Task& currentTask)
	{
		std::unique_lock<std::mutex> lock(m_queueMutex);
		m_queueCondition.wait
		(
			lock,
			// зачем нужно проверять stop_requested?
			[this, &stopToken] { return stopToken.stop_requested() || m_stopFlag.load() || !m_taskQueue.empty(); }
		);

		if (m_stopFlag.load() || stopToken.stop_requested())
		{
			return false;
		}

		// очередь как будто не может быть пустой
		if (!m_taskQueue.empty())
		{
			currentTask = std::move(m_taskQueue.front());
			m_taskQueue.pop();
		}

		return true;
	}

	void Run(std::stop_token stopToken)
	{
		while (!m_stopFlag.load())
		{
			Task currentTask;

			bool shouldStop = !WaitNewTaskOrStop(stopToken, currentTask);
			if (shouldStop)
			{
				break;
			}

			if (currentTask)
			{
				try
				{
					currentTask();
				}
				catch(...)
				{}
			}

			std::lock_guard<std::mutex> lock(m_queueMutex);
			m_isCompleted = m_taskQueue.empty();
			// возмно убрать lock (или возможно сдеать ее под локом Wait ф-ии)
			// notify методы можно вызывать без блокировки
			m_finishedCondition.notify_all();
		}
	}
};