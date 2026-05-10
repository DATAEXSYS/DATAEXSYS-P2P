#pragma once

#include <any>
#include <functional>
#include <future>
#include <utility>

class Task {
private:
    std::any input;
    std::function<std::any(std::any)> executor;
    std::promise<std::any> promise;

public:
    // -------------------------------------------------
    // Boundary: Templated constructor
    // -------------------------------------------------
    template<typename F, typename I>
    Task(I in, F fn)
        : input(std::move(in)),
          executor([fn = std::move(fn)](std::any any_in) -> std::any {
              using InputT = std::decay_t<I>;
              return fn(std::any_cast<InputT>(any_in));
          })
    {}

    // Constructor for no-input tasks
    template<typename F>
    Task(F fn)
        : executor([fn = std::move(fn)](std::any) -> std::any {
              return fn();
          })
    {}

    // Runtime type-erased constructors (for internal use)
    Task(std::any in, std::function<std::any(std::any)> fn);
    Task(std::function<std::any()> fn);

    // Default move support
    Task(Task&&) noexcept;
    Task& operator=(Task&&) noexcept;

    // No copying
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    // -------------------------------------------------
    // Execution (TaskQueue calls this)
    // -------------------------------------------------
    void execute();

    // -------------------------------------------------
    // Internal: expose future to TaskHandle
    // -------------------------------------------------
    std::future<std::any> getFuture();

    bool valid() const noexcept;
};