#include "runtime/taskhandle.h"

TaskHandle::TaskHandle(std::future<std::any> f)
    : future(std::move(f)) 
{}

bool TaskHandle::ready() const {
    return future.valid() &&
           future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

bool TaskHandle::valid() const noexcept {
    return future.valid();
}
