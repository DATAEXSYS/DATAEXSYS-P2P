#pragma once

#include "runtime/taskqueue.h"
#include "runtime/taskhandle.h"
#include "core/task.h"
#include <memory>

class TaskSystem {
private:
    TaskQueue queue;

    // Internal submission logic
    TaskHandle submitInternal(Task task);

public:
    TaskSystem() = default;

    void start();
    void stop();

    // -------------------------------------------------
    // Boundary: Templated submit (User Friendly)
    // -------------------------------------------------
    
    // Submit with input
    template<typename I, typename F>
    TaskHandle submit(I input, F executor) {
        // Abstracted ownership transition: internal Task creation
        return submitInternal(Task(std::move(input), std::move(executor)));
    }

    // Submit without input
    template<typename F>
    TaskHandle submit(F executor) {
        return submitInternal(Task(std::move(executor)));
    }

    // Disable raw task submission to enforce abstracted ownership
    TaskHandle submit(Task task) = delete;
};