#ifndef TASK_H
#define TASK_H

#include <functional>
#include <any>
#include <chrono>
#include <memory>
#include <string>

class Task {
public:
    using TaskFunc = std::function<std::any(std::any)>;

    Task() = default;

    Task(TaskFunc fn,
         std::any input = {},
         std::chrono::steady_clock::time_point runAt =
             std::chrono::steady_clock::now(),
         std::string id = "")
        : function(fn),
          inputData(input),
          runAt(runAt),
          taskId(id) {}

    // Execute task
    void execute() {
        if (function) {
            result = function(inputData);
        }
    }

    // Get result
    std::any getResult() const {
        return result;
    }

    // Get scheduled time
    std::chrono::steady_clock::time_point getRunTime() const {
        return runAt;
    }

    // Optional ID
    std::string getId() const {
        return taskId;
    }

    // Comparator for priority queue (earlier first)
    bool operator>(const Task &other) const {
        return runAt > other.runAt;
    }

private:
    TaskFunc function;

    std::any inputData;
    std::any result;

    std::chrono::steady_clock::time_point runAt;
    std::string taskId;
};

#endif