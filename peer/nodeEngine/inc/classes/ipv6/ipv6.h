#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

struct ipv6
{
    std::array<uint8_t, 16> address;

    explicit ipv6(const std::array<uint8_t, 16> &addr) : address(addr) {}
    ipv6() = default;

    explicit ipv6(std::string_view text);

    bool is_loopback() const;
    bool is_multicast() const;
    bool is_link_local() const;
    bool is_global() const;

    std::string to_string() const;
};