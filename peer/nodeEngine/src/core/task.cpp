#include "core/task.h"

Task::Task(std::any in, std::function<std::any(std::any)> fn)
    : input(std::move(in)),
      executor(std::move(fn))
{}

Task::Task(std::function<std::any()> fn)
    : executor([fn = std::move(fn)](std::any) -> std::any {
          return fn();
      })
{}

Task::Task(Task&& other) noexcept
    : input(std::move(other.input)),
      executor(std::move(other.executor)),
      promise(std::move(other.promise))
{}

Task& Task::operator=(Task&& other) noexcept {
    if (this != &other) {
        input = std::move(other.input);
        executor = std::move(other.executor);
        promise = std::move(other.promise);
    }
    return *this;
}

void Task::execute() {
    if (executor) {
        try {
            std::any result = executor(input);
            promise.set_value(std::move(result));
        } catch (...) {
            promise.set_exception(std::current_exception());
        }
    }
}

std::future<std::any> Task::getFuture() {
    return promise.get_future();
}

bool Task::valid() const noexcept {
    return static_cast<bool>(executor);
}
