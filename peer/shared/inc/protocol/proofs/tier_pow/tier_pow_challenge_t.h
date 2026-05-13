#pragma once

#include <stdint.h>
#include <stdbool.h>
// Add necessary includes

typedef struct __attribute__((aligned(4))) {
    uint256 challenge;   // 32 bytes
    uint8_t complexity;  // 1 byte
    uint64_t challenge_id; // 8 bytes
    uint8_t reserved[3]; // padding to make 32-bit multiple
} tier_pow_challenge_t;
