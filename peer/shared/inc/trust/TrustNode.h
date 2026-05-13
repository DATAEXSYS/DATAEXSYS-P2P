#ifndef TRUST_NODE_H
#define TRUST_NODE_H

#if !defined(__linux__)
#error "This project is optimized for Linux systems only"
#endif

#include <stdint.h>
#include <shared/inc/datatypes/ipv6.h>

typedef struct __attribute__((aligned(4))) {
    ipv6_t nodeid;  //128 bits for IPv6 address
    uint32_t trust; // 32-bit trust score
    uint32_t acks; // 32 bits
    uint32_t nacks; // 32 bits
} TrustNode;

#endif // TRUST_NODE_H  