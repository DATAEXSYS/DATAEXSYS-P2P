#include "runtime/core/worker.h"
#include "runtime/core/scheduler.h"

namespace runtime {

Worker::Worker(size_t id, Scheduler* s)
    : worker_id(id), scheduler(s), rng(std::random_device{}()) {}

void Worker::run(std::stop_token stop_token) {
    while (!stop_token.stop_requested()) {
        auto task = get_task();
        
        if (task) {
            execute_task(*task);
        } else {
            // No work found, wait for notification
            scheduler->wait_for_tasks(stop_token);
        }
    }
}

std::optional<ExecutorTask> Worker::get_task() {
    // 1. Try local queue (LIFO - Cache friendly)
    if (auto task = local_queue.pop()) {
        return task;
    }

    // 2. Try global queue
    if (auto task = scheduler->pop_global()) {
        return task;
    }

    // 3. Work stealing (FIFO - Fairness)
    size_t count = scheduler->worker_count();
    if (count > 1) {
        std::uniform_int_distribution<size_t> dist(0, count - 1);
        size_t victim_id = dist(rng);
        
        if (victim_id != worker_id) {
            if (auto task = scheduler->get_worker(victim_id)->steal()) {
                return task;
            }
        }
    }

    return std::nullopt;
}

void Worker::execute_task(ExecutorTask& task) {
    try {
        task();
    } catch (...) {
        // Log exception with task.get_location()
    }
}

} // namespace runtime
