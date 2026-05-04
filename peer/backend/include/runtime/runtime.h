#pragma once

#include "runtime/core/scheduler.h"
#include "runtime/task/coro.h"

namespace runtime {

/**
 * @brief The public interface for the async runtime.
 */
class Runtime {
public:
    static Runtime& instance() {
        static Runtime r;
        return r;
    }

    /**
     * @brief Spawn a new fire-and-forget task.
     */
    template <typename F>
    void spawn(F&& f, std::source_location loc = std::source_location::current()) {
        scheduler.submit(ExecutorTask(std::forward<F>(f), loc));
    }

    /**
     * @brief Schedule a coroutine task.
     */
    template <typename T>
    void spawn(ResumableTask<T> task, std::source_location loc = std::source_location::current()) {
        auto handle = task.handle;
        task.handle = nullptr; // Take ownership
        scheduler.submit(ExecutorTask([handle]() {
            if (handle && !handle.done()) {
                handle.resume();
            }
            if (handle && handle.done()) {
                handle.destroy();
            }
        }, loc));
    }

    /**
     * @brief Awaitable that schedules a coroutine onto the runtime.
     */
    struct ScheduleAwaitable {
        bool await_ready() const noexcept { return false; }
        void await_suspend(std::coroutine_handle<> h) noexcept {
            Runtime::instance().scheduler.submit(ExecutorTask([h]() {
                h.resume();
            }));
        }
        void await_resume() const noexcept {}
    };

    static auto schedule() {
        return ScheduleAwaitable{};
    }

private:
    Runtime() : scheduler(std::thread::hardware_concurrency()) {}
    Scheduler scheduler;
};

// Implementation of Yield from coro.h
inline void Yield::await_suspend(std::coroutine_handle<> h) noexcept {
    Runtime::instance().spawn([h]() { h.resume(); });
}

} // namespace runtime
