#include "runtime/taskqueue.h"

TaskQueue::TaskQueue() : running(false) {}

TaskQueue::~TaskQueue() {
    stop();
}

void TaskQueue::start() {
    running = true;

    worker = std::thread([this]() {
        this->processLoop();
    });
}

void TaskQueue::stop() {
    {
        std::lock_guard<std::mutex> lock(m);
        if (!running) return;
        running = false;
    }

    cv.notify_all();

    if (worker.joinable()) {
        worker.join();
    }
}

void TaskQueue::push(std::shared_ptr<Task> task) {
    {
        std::lock_guard<std::mutex> lock(m);
        queue.push(std::move(task));
    }

    cv.notify_one();
}

void TaskQueue::processLoop() {
    while (true) {
        std::shared_ptr<Task> task;

        {
            std::unique_lock<std::mutex> lock(m);

            cv.wait(lock, [this]() {
                return !queue.empty() || !running;
            });

            if (!running && queue.empty()) {
                return;
            }

            task = std::move(queue.front());
            queue.pop();
        }

        task->execute();
    }
}