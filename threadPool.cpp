#include "threadPool.h"

namespace http
{
    ThreadPool::ThreadPool(size_t numOfThreads) : stop(false)
    {
        for (size_t i = 0; i < numOfThreads; i++)
        {
            threads.emplace_back([this]
                                 {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mutex);
                        condition.wait(lock, [this] {
                            return stop || !tasks.empty();
                        });
                        if (stop && tasks.empty()) return;
                        if (!tasks.empty()) {
                            task = std::move(tasks.front());
                            tasks.pop();
                        }
                    }
                    task(); 
                } });
        }
    };

    ThreadPool::~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            stop = true;
        }

        condition.notify_all();
        for (std::thread &thread : threads)
        {
            thread.join();
        }
    }

    void ThreadPool::enqueueTask(std::function<void()> task)
    {
        {
            std::unique_lock<std::mutex> lock(mutex);
            tasks.push(std::move(task));
        }
        condition.notify_all();
    }
}