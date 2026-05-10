#pragma once

#include <any>
#include <future>
#include <utility>
#include <chrono>

class TaskHandle {
private:
    std::future<std::any> future;

public:
    TaskHandle() = default;

    explicit TaskHandle(std::future<std::any> f);

    // -------------------------------------------------
    // Boundary: Templated getter
    // -------------------------------------------------
    template<typename T>
    T get() {
        std::any result = future.get(); // blocks until ready
        return std::any_cast<T>(result);
    }

    // -------------------------------------------------
    // Status checks
    // -------------------------------------------------
    bool ready() const;
    bool valid() const noexcept;

    // Move only
    TaskHandle(TaskHandle&&) noexcept = default;
    TaskHandle& operator=(TaskHandle&&) noexcept = default;
    TaskHandle(const TaskHandle&) = delete;
    TaskHandle& operator=(const TaskHandle&) = delete;
};