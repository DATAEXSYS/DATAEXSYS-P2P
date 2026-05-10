#include "classes/ipv6_pack/ipv6_pack.h"



// ============================================================
// ipv6_pack
// ============================================================

void ipv6_pack::add(const ipv6& addr)
{
    addresses.push_back(addr);
}

void ipv6_pack::add(ipv6&& addr)
{
    addresses.push_back(std::move(addr));
}

std::vector<ipv6>
ipv6_pack::global_addresses() const
{
    std::vector<ipv6> result;

    for (const auto& addr : addresses)
    {
        if (addr.is_global())
        {
            result.push_back(addr);
        }
    }

    return result;
}

ipv6 ipv6_pack::primary_global() const
{
    for (const auto& addr : addresses)
    {
        if (addr.is_global())
        {
            return addr;
        }
    }

    throw std::runtime_error(
        "No global IPv6 address found"
    );
}