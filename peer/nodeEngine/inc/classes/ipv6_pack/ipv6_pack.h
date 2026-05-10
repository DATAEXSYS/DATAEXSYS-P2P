#pragma once
#include <vector>
#include "ipv6/ipv6.h"



struct ipv6_pack
{
    std::vector<ipv6> addresses;

    void add(const ipv6& addr);
    void add(ipv6&& addr);

    std::vector<ipv6> global_addresses() const;

    // returns first valid global address
    // throws if none exist
    ipv6 primary_global() const;
};