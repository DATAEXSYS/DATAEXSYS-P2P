#pragma once

#include <string>
#include "classes/ipv6/ipv6.h"

struct Certificate {
    std::string name;
    ipv6 ipv6_address;
    ipv6 issuer;
    std::array<uint8_t, 32> public_key;
    std::array<uint8_t, 32> signature;
    std::string validFrom;
    std::string validTo;

};