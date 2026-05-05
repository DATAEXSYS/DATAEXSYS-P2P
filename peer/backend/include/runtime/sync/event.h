#pragma once

#include <coroutine>
#include <atomic>
#include <vector>
#include <mutex>

namespace runtime {

/**
 * @brief A coroutine-compatible manual reset event.
 * 
 * Allows coroutines to suspend until the event is signaled.
 */
class AsyncEvent {
public:
    AsyncEvent() : ready_(false) {}

    /**
     * @brief Signals the event and resumes all waiting coroutines.
     */
    void set() {
        std::vector<std::coroutine_handle<>> to_resume;
        {
            std::lock_guard<std::mutex> lock(mtx_);
            ready_ = true;
            to_resume = std::move(waiters_);
        }
        
        for (auto h : to_resume) {
            if (h) h.resume();
        }
    }

    /**
     * @brief Resets the event to non-signaled state.
     */
    void reset() {
        std::lock_guard<std::mutex> lock(mtx_);
        ready_ = false;
    }

    /**
     * @brief Awaitable structure for the event.
     */
    struct Awaiter {
        AsyncEvent& event;

        bool await_ready() const noexcept {
            return event.ready_.load();
        }

        void await_suspend(std::coroutine_handle<> h) noexcept {
            std::lock_guard<std::mutex> lock(event.mtx_);
            if (event.ready_) {
                // Race condition: event became ready just before we suspended
                h.resume();
            } else {
                event.waiters_.push_back(h);
            }
        }

        void await_resume() const noexcept {}
    };

    auto operator co_await() noexcept {
        return Awaiter{*this};
    }

private:
    std::atomic<bool> ready_;
    std::mutex mtx_;
    std::vector<std::coroutine_handle<>> waiters_;
};

} // namespace runtime
