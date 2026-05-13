#pragma once

#include <stdint.h>
#include "core/datatypes/ipv6.h"
#include <stdbool.h>
// Add necessary includes

typedef struct __attribute__((aligned(32))) {
    ipv6_t nodeid;
    uint32_t trust;
    uint32_t acks;
    uint32_t nacks;
    uint8_t reserved[19];
} TrustNode;
