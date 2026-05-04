#ifndef NODE_IDENTITY_H
#define NODE_IDENTITY_H

#include <string>
#include <array>
#include <cstdint>

struct NodeIdentity {

    std::string ipv6;

    // X25519
    std::array<uint8_t, 32> x25519_private{};
    std::array<uint8_t, 32> x25519_public{};

    // Ed25519
    std::array<uint8_t, 32> ed25519_private{};
    std::array<uint8_t, 32> ed25519_public{};

    // metadata (IMPORTANT FOR SYSTEM DESIGN)
    uint64_t created_at_unix = 0;
    bool is_valid = false;
};

#endif