#include "runtime/core/scheduler.h"

namespace runtime {

thread_local Worker* Scheduler::current_worker = nullptr;

Scheduler::Scheduler(size_t thread_count) {
    if (thread_count == 0) thread_count = 1;

    workers.reserve(thread_count);
    threads.reserve(thread_count);

    for (size_t i = 0; i < thread_count; ++i) {
        workers.push_back(std::make_unique<Worker>(i, this));
    }

    for (size_t i = 0; i < thread_count; ++i) {
        threads.emplace_back([this, i](std::stop_token st) {
            current_worker = workers[i].get();
            current_worker->run(st);
        });
    }
}

Scheduler::~Scheduler() {
    shutdown();
}

void Scheduler::submit(ExecutorTask task) {
    if (current_worker) {
        current_worker->push(std::move(task));
    } else {
        std::lock_guard lock(global_mtx);
        global_queue.push_back(std::move(task));
    }
    
    // Increment task count and notify
    task_count.fetch_add(1, std::memory_order_release);
    task_count.notify_one();
}

std::optional<ExecutorTask> Scheduler::pop_global() {
    std::lock_guard lock(global_mtx);
    if (global_queue.empty()) return std::nullopt;
    
    auto task = std::move(global_queue.back());
    global_queue.pop_back();
    return task;
}

void Scheduler::wait_for_tasks(std::stop_token st) {
    uint64_t current = task_count.load(std::memory_order_acquire);
    while (!st.stop_requested()) {
        task_count.wait(current, std::memory_order_acquire);
        uint64_t next = task_count.load(std::memory_order_acquire);
        if (next != current) break;
    }
}

void Scheduler::shutdown() {
    for (auto& t : threads) {
        t.request_stop();
    }
    task_count.fetch_add(1, std::memory_order_release);
    task_count.notify_all();
    
    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }
    threads.clear();
}

} // namespace runtime
