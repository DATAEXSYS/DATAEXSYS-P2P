#pragma once

#include "runtime/core/worker.h"
#include "runtime/task/executor_task.h"

#include <vector>
#include <memory>
#include <thread>
#include <stop_token>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace runtime {

/**
 * @brief The centralized execution unit for the distributed system runtime.
 */
class Scheduler {
public:
    explicit Scheduler(size_t thread_count = std::thread::hardware_concurrency());
    ~Scheduler();

    Scheduler(const Scheduler&) = delete;
    Scheduler& operator=(const Scheduler&) = delete;

    /**
     * @brief Submit a task to the runtime.
     */
    void submit(ExecutorTask task);

    /**
     * @brief Shutdown the runtime gracefully.
     */
    void shutdown();

    // Internal methods for workers
    Worker* get_worker(size_t id) { return workers[id].get(); }
    size_t worker_count() const { return workers.size(); }
    
    // Global queue access for workers
    std::optional<ExecutorTask> pop_global();

    // Parking/Unparking
    void notify_one();
    void wait_for_tasks(std::stop_token st);

private:
    static thread_local Worker* current_worker;

    std::vector<std::unique_ptr<Worker>> workers;
    std::vector<std::jthread> threads;
    
    // Global fallback queue
    std::mutex global_mtx;
    std::vector<ExecutorTask> global_queue;

    std::atomic<uint64_t> idle_count{0};
    std::atomic<uint64_t> task_count{0}; // For atomic wait/notify
};

} // namespace runtime
