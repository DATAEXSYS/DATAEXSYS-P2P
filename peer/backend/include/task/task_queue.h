#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <vector>

class TaskQueue {
public:
    using Task = std::function<void()>;

    TaskQueue(size_t threadCount = 2);
    ~TaskQueue();

    // push task (async execution)
    void push(Task task);

    // stop queue
    void stop();

private:
    void workerLoop();

    std::vector<std::thread> workers;

    std::queue<Task> tasks;
    std::mutex mtx;
    std::condition_variable cv;

    bool running = true;
};

#endif