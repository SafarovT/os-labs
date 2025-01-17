#pragma once
#include <atomic>
#include <condition_variable>
#include <exception>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
public:
    using Task = std::function<void()>;

    explicit ThreadPool(unsigned int threadCount)
    {
        for (unsigned int i = 0; i < threadCount; ++i)
        {
            m_workers.emplace_back([this](std::stop_token stopToken) { WorkerLoop(stopToken);   });
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ~ThreadPool()
    {
        Stop();
    }

    void Dispatch(Task task)
    {
        {
            std::scoped_lock lock(m_queueMutex);
            if (m_isStopped)
            {
                return;
            }
            m_taskQueue.emplace(std::move(task));
        }
        m_taskNotifier.notify_one();
    }

    void Wait()
    {
        std::unique_lock lock(m_queueMutex);
        m_completionNotifier.wait(lock, [this]() { return m_taskQueue.empty() && m_activeTaskCount == 0; });
    }

    void Stop()
    {
        m_isStopped.store(true);
        m_taskNotifier.notify_all();
    }

private:
    void WorkersLoop(const std::stop_token& stopToken)
    {
        while (!stopToken.stop_requested())
        {
            Task currentTask;
            {
                std::unique_lock lock(m_queueMutex);
                m_taskNotifier.wait(lock, [this]() { return m_isStopped || !m_taskQueue.empty(); });

                if (m_isStopped && m_taskQueue.empty())
                {
                    return;
                }

                currentTask = std::move(m_taskQueue.front());
                m_taskQueue.pop();
                ++m_activeTaskCount;
            }

            ExecuteTask(currentTask);
        }
    }

    void ExecuteTask(Task& task)
    {
        try
        {
            task();
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error in task execution: " << e.what() << std::endl;
        }

        std::scoped_lock lock(m_queueMutex);
        --m_activeTaskCount;
        if (m_taskQueue.empty() && m_activeTaskCount == 0)
        {
            m_completionNotifier.notify_all();
        }
    }

    std::queue<Task> m_taskQueue;
    std::mutex m_queueMutex;
    std::condition_variable m_taskNotifier;
    std::condition_variable m_completionNotifier;
    std::atomic<bool> m_isStopped{ false };
    std::atomic<int> m_activeTaskCount{ 0 };
    std::vector<std::jthread> m_workers;
};
