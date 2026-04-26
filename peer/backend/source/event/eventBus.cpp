#include "event/EventBus.h"

// ================= SUBSCRIBE =================
void EventBus::subscribe(const std::string& eventType, Handler handler) {
    std::lock_guard<std::mutex> lock(mtx);
    subscribers[eventType].push_back(handler);
}

// ================= EMIT (SYNC) =================
void EventBus::emit(const Event& event) {
    execute(event);
}

// ================= EMIT (ASYNC) =================
void EventBus::emitAsync(const Event& event) {
    std::thread([this, event]() {
        execute(event);
    }).detach();
}

// ================= EXECUTE =================
void EventBus::execute(const Event& event) {
    std::vector<Handler> handlers;

    {
        std::lock_guard<std::mutex> lock(mtx);

        if (subscribers.find(event.getType()) != subscribers.end()) {
            handlers = subscribers[event.getType()];
        }
    }

    // execute outside lock (IMPORTANT)
    for (auto& handler : handlers) {
        handler(event);
    }
}