#pragma once

#include <coroutine>
#include <exception>
#include <utility>
#include <optional>

namespace runtime {

/**
 * @brief A lightweight coroutine task designed for the centralized runtime.
 * 
 * ResumableTask is move-only and does not start executing until submitted to the scheduler.
 */
template<typename T = void>
struct ResumableTask {
    struct promise_type {
        ResumableTask get_return_object() {
            return ResumableTask(handle_type::from_promise(*this));
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        
        void unhandled_exception() {
            exception = std::current_exception();
        }

        void return_value(T value) {
            result = std::move(value);
        }

        std::optional<T> result;
        std::exception_ptr exception;
    };

    using handle_type = std::coroutine_handle<promise_type>;

    explicit ResumableTask(handle_type h) : handle(h) {}
    ~ResumableTask() {
        if (handle) handle.destroy();
    }

    ResumableTask(const ResumableTask&) = delete;
    ResumableTask& operator=(const ResumableTask&) = delete;

    ResumableTask(ResumableTask&& other) noexcept : handle(std::exchange(other.handle, nullptr)) {}
    ResumableTask& operator=(ResumableTask&& other) noexcept {
        if (this != &other) {
            if (handle) handle.destroy();
            handle = std::exchange(other.handle, nullptr);
        }
        return *this;
    }

    handle_type handle;
};

// Specialization for void
template<>
struct ResumableTask<void> {
    struct promise_type {
        ResumableTask get_return_object() {
            return ResumableTask(handle_type::from_promise(*this));
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        
        void unhandled_exception() {
            exception = std::current_exception();
        }

        void return_void() {}

        std::exception_ptr exception;
    };

    using handle_type = std::coroutine_handle<promise_type>;

    explicit ResumableTask(handle_type h) : handle(h) {}
    ~ResumableTask() {
        if (handle) handle.destroy();
    }

    ResumableTask(const ResumableTask&) = delete;
    ResumableTask& operator=(const ResumableTask&) = delete;

    ResumableTask(ResumableTask&& other) noexcept : handle(std::exchange(other.handle, nullptr)) {}
    ResumableTask& operator=(ResumableTask&& other) noexcept {
        if (this != &other) {
            if (handle) handle.destroy();
            handle = std::exchange(other.handle, nullptr);
        }
        return *this;
    }

    handle_type handle;
};

/**
 * @brief Awaitable that allows a coroutine to yield execution to the scheduler.
 */
struct Yield {
    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> h) noexcept; 
    void await_resume() const noexcept {}
};

} // namespace runtime
