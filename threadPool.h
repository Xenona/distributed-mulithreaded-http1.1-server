#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <functional>

namespace http
{

    class ThreadPool
    {
    public:
        ThreadPool(size_t numOfThreads);

        ~ThreadPool();

        void enqueueTask(std::function<void()> task);

    private:
        std::vector<std::thread> threads;
        std::queue<std::function<void()>> tasks;
        std::mutex mutex;
        std::condition_variable condition;
        bool stop;
    };

}