#include "threadPool.h"
#include <iostream>

namespace http
{

    using namespace std;
    ThreadPool::ThreadPool(size_t numOfThreads) : stop(false)
    {
        for (size_t i = 0; i < numOfThreads; i++)
        {
            threads.emplace_back([this]
                                 {
                while (true) {
                    function<void()> task;
                    {
                        unique_lock<std::mutex> lock(mutex);
                        // cout << "WAITING ON LOCK" << endl;
                        condition.wait(lock, [this] {
                            return stop || !tasks.empty();
                        });
                        // cout << "WAITING --- DONE" << endl;
                        if (stop && tasks.empty()) return;
                        if (!tasks.empty()) {
                            task = std::move(tasks.front());
                            tasks.pop();
                        }
                        // cout << "POPPED" << endl;
                    }
                    task(); 
                    cout << "TASK EXECUTED" << endl;
                } });
        }
    };

    ThreadPool::~ThreadPool()
    {
        {
            unique_lock<std::mutex> lock(mutex);
            stop = true;
        }

        condition.notify_all();
        for (thread &thread : threads)
        {
            thread.join();
        }
    }

    void ThreadPool::enqueueTask(std::function<void()> task)
    {
        {
            // cout << "task 1" << endl;
            unique_lock<std::mutex> lock(mutex);
            tasks.push(std::move(task));
            cout << "task pushed and queued" << endl;
        }
        condition.notify_all();
        // cout << "task notified one" << endl;
    }
}