#pragma once

#include "runtime/core/work_queue.h"
#include "runtime/task/executor_task.h"

#include <stop_token>
#include <vector>
#include <random>
#include <thread>

namespace runtime {

class Scheduler;

/**
 * @brief Represents a single worker thread in the runtime.
 */
class Worker {
public:
    Worker(size_t id, Scheduler* scheduler);

    void run(std::stop_token stop_token);

    // Push task to local queue (Owner only)
    void push(ExecutorTask task) {
        local_queue.push(std::move(task));
    }

    // Steal task from this worker (Thieves only)
    std::optional<ExecutorTask> steal() {
        return local_queue.steal();
    }

    size_t get_id() const { return worker_id; }

private:
    std::optional<ExecutorTask> get_task();
    void execute_task(ExecutorTask& task);

    size_t worker_id;
    Scheduler* scheduler;
    WorkStealingQueue<ExecutorTask> local_queue;
    std::mt19937 rng;
};

} // namespace runtime
