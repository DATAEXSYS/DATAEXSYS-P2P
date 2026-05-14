#pragma once
#include <functional>
#include <string>
#include <atomic>

namespace network {
    void start_receiver(std::function<void(const std::string& ip, const std::string& message)> on_message_received, std::atomic<bool>& running);
}
