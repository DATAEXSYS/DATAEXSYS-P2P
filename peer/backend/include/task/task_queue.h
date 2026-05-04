// task_queue.h

#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include "task.h"

#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class TaskQueue {
public:
    TaskQueue(size_t threadCount = std::thread::hardware_concurrency());
    ~TaskQueue();

    // non-copyable
    TaskQueue(const TaskQueue&) = delete;
    TaskQueue& operator=(const TaskQueue&) = delete;

    // add task
    void push(const Task& task);

    // stop queue
    void stop();

private:
    void workerLoop();

private:
    struct TaskCompare {
        bool operator()(const Task& a, const Task& b) const {
            return a.getRunTime() > b.getRunTime();
        }
    };

    std::priority_queue<
        Task,
        std::vector<Task>,
        TaskCompare
    > tasks;

    std::vector<std::thread> workers;

    std::mutex mtx;
    std::condition_variable cv;

    std::atomic<bool> running;
};

#endif