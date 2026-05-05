#include "runtime/runtime.h"
#include <iostream>
#include <vector>
#include <numeric>
#include <string>

using namespace runtime;

/**
 * 1. BASIC COROUTINE TASK
 * A simple fire-and-forget coroutine.
 */
ResumableTask<> basic_coro_task(int id) {
    co_await Runtime::schedule(); // Ensure we are on a worker thread
    std::cout << "[Task " << id << "] Basic coroutine started on thread: " 
              << std::this_thread::get_id() << std::endl;
    co_return;
}

/**
 * 2. ADVANCED COROUTINE WITH YIELDING
 * Demonstrates yielding execution to the scheduler.
 */
ResumableTask<> yielding_task() {
    co_await Runtime::schedule();
    std::cout << "[YieldTask] Part 1 executing..." << std::endl;
    
    co_await Yield{}; // Yield back to scheduler to allow other tasks to run
    
    std::cout << "[YieldTask] Part 2 executing after yield on thread: " 
              << std::this_thread::get_id() << std::endl;
    co_return;
}

/**
 * 3. EXCEPTION HANDLING TASK
 * Demonstrates how the runtime handles task failures.
 */
void failing_task() {
    std::cout << "[FailingTask] About to throw an exception..." << std::endl;
    throw std::runtime_error("Deliberate task failure for demonstration");
}

int main() {
    auto& rt = Runtime::instance();
    std::cout << "[Main] Runtime initialized. Starting demonstration...\n" << std::endl;

    // --- TASK 1: SIMPLE LAMBDA ---
    rt.spawn([]() {
        std::cout << "[LambdaTask] Simple fire-and-forget lambda executing." << std::endl;
    });

    // --- TASK 2: STATEFUL MOVE-ONLY LAMBDA ---
    auto large_data = std::make_unique<std::vector<int>>(1000, 42);
    rt.spawn([data = std::move(large_data)]() {
        std::cout << "[MoveOnlyTask] Large data processed. Size: " << data->size() << std::endl;
    });

    // --- TASK 3: BASIC COROUTINE ---
    rt.spawn(basic_coro_task(101));

    // --- TASK 4: YIELDING COROUTINE ---
    rt.spawn(yielding_task());

    // --- TASK 5: FAILING TASK ---
    rt.spawn(failing_task);

    std::cout << "\n[Main] All tasks submitted. Waiting for completion...\n" << std::endl;

    // Keep main alive to allow workers to finish
    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::cout << "\n[Main] Demonstration finished. Shutting down runtime." << std::endl;
    return 0;
}