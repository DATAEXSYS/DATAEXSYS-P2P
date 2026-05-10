// ipv6_address.cpp

#include "classes/ipv6/ipv6.h"

#include <arpa/inet.h>
#include <stdexcept>



// ============================================================
// ipv6_address
// ============================================================

ipv6::ipv6(std::string_view text)
{
    if (
        inet_pton(
            AF_INET6,
            std::string(text).c_str(),
            address.data()
        ) != 1
    )
    {
        throw std::runtime_error(
            "Invalid IPv6 address"
        );
    }
}

bool ipv6::is_loopback() const
{
    for (size_t i = 0; i < 15; ++i)
    {
        if (address[i] != 0)
        {
            return false;
        }
    }

    return address[15] == 1;
}

bool ipv6::is_multicast() const
{
    // ff00::/8
    return address[0] == 0xFF;
}

bool ipv6::is_link_local() const
{
    // fe80::/10
    return
        address[0] == 0xFE &&
        ((address[1] & 0xC0) == 0x80);
}

bool ipv6::is_global() const
{
    return
        !is_loopback() &&
        !is_multicast() &&
        !is_link_local();
}

std::string ipv6::to_string() const
{
    char buffer[INET6_ADDRSTRLEN]{};

    if (
        inet_ntop(
            AF_INET6,
            address.data(),
            buffer,
            INET6_ADDRSTRLEN
        ) == nullptr
    )
    {
        throw std::runtime_error(
            "Failed to convert IPv6 address to string"
        );
    }

    return std::string(buffer);
}



