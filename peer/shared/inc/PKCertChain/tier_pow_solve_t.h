#pragma once

#include <stdint.h>
#include <stdbool.h>
// Add necessary includes

typedef struct __attribute__((aligned(4)))
{
    uint64_t nonce;     // 8 bytes
    uint8_t complexity; // 1 byte
    uint64_t challenge_id; // 8 bytes
    uint8_t reserved[3];
} tier_pow_solve_t;
