#pragma once
#include <string>

namespace network {
    bool send_message(const std::string& dest_ip, const std::string& message);
}
