#include "runtime/tasksystem.h"

void TaskSystem::start() {
    queue.start();
}

void TaskSystem::stop() {
    queue.stop();
}

TaskHandle TaskSystem::submitInternal(Task task) {
    // 1. extract future BEFORE moving task
    std::future<std::any> future = task.getFuture();

    // 2. push task into execution queue
    // Note: TaskQueue expects std::shared_ptr<Task>
    queue.push(std::make_shared<Task>(std::move(task)));

    // 3. return handle to user
    return TaskHandle(std::move(future));
}