#include "task/task_queue.h"

TaskQueue::TaskQueue(size_t threadCount) {
    running = true;

    for (size_t i = 0; i < threadCount; i++) {
        workers.emplace_back(&TaskQueue::workerLoop, this);
    }
}

TaskQueue::~TaskQueue() {
    stop();
}

void TaskQueue::push(Task task) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        tasks.push(task);
    }
    cv.notify_one();
}

void TaskQueue::stop() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        running = false;
    }

    cv.notify_all();

    for (auto &t : workers) {
        if (t.joinable())
            t.join();
    }
}

void TaskQueue::workerLoop() {
    while (true) {
        Task task;

        {
            std::unique_lock<std::mutex> lock(mtx);

            cv.wait(lock, [&]() {
                return !tasks.empty() || !running;
            });

            if (!running && tasks.empty())
                return;

            task = tasks.front();
            tasks.pop();
        }

        task(); // execute
    }
}