#ifndef NODE_IDENTITY_H
#define NODE_IDENTITY_H

#include <string>
#include <array>

struct NodeIdentity {

    std::string ipv6;

    // X25519 (32 bytes each)
    std::array<uint8_t, 32> x25519_private;
    std::array<uint8_t, 32> x25519_public;

    // Ed25519 (32 bytes private seed, 32 public)
    std::array<uint8_t, 32> ed25519_private;
    std::array<uint8_t, 32> ed25519_public;
};

#endif