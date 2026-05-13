#pragma once

#include <stdint.h>
#include <stdbool.h>
// Add necessary includes

typedef struct __attribute__((aligned(32))) {
    uint8_t nodeid;
    uint32_t trust;
    uint32_t acks;
    uint32_t nacks;
    uint8_t reserved[19];
} TrustNode;
