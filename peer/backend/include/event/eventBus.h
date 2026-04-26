#ifndef EVENTBUS_H
#define EVENTBUS_H

#include "Event.h"

#include <functional>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <thread>

class EventBus {
public:
    using Handler = std::function<void(const Event&)>;

    // Subscribe to an event type
    void subscribe(const std::string& eventType, Handler handler);

    // Emit event (sync)
    void emit(const Event& event);

    // Emit event asynchronously
    void emitAsync(const Event& event);

private:
    std::unordered_map<std::string, std::vector<Handler>> subscribers;
    std::mutex mtx;

    // internal execution
    void execute(const Event& event);
};

#endif