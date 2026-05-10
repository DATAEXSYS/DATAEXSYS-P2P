#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>

#include "core/task.h"

class TaskQueue {
private:
    std::queue<std::shared_ptr<Task>> queue;

    std::mutex m;
    std::condition_variable cv;

    std::thread worker;
    std::atomic<bool> running;

public:
    TaskQueue();
    ~TaskQueue();

    void start();
    void stop();

    void push(std::shared_ptr<Task> task);

private:
    void processLoop();
};