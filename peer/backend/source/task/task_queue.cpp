// task_queue.cpp

#include "task/task_queue.h"

TaskQueue::TaskQueue(size_t threadCount)
    : running(true)
{
    if (threadCount == 0)
        threadCount = 1;

    workers.reserve(threadCount);

    for (size_t i = 0; i < threadCount; ++i) {
        workers.emplace_back(&TaskQueue::workerLoop, this);
    }
}

TaskQueue::~TaskQueue() {
    stop();
}

void TaskQueue::push(const Task& task) {
    {
        std::lock_guard<std::mutex> lock(mtx);

        if (!running)
            return;

        tasks.push(task);
    }

    cv.notify_one();
}

void TaskQueue::stop() {
    bool expected = true;

    if (!running.compare_exchange_strong(expected, false))
        return;

    cv.notify_all();

    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }

    workers.clear();
}

void TaskQueue::workerLoop() {
    while (running) {
        Task task;

        {
            std::unique_lock<std::mutex> lock(mtx);

            cv.wait(lock, [this]() {
                return !tasks.empty() || !running;
            });

            if (!running && tasks.empty())
                return;

            auto now = std::chrono::steady_clock::now();
            auto nextRun = tasks.top().getRunTime();

            // wait until scheduled execution time
            if (nextRun > now) {
                cv.wait_until(lock, nextRun);

                if (!running)
                    return;

                if (tasks.empty())
                    continue;
            }

            task = tasks.top();
            tasks.pop();
        }

        try {
            task.execute();
        }
        catch (...) {
            // optional logging
        }
    }
}