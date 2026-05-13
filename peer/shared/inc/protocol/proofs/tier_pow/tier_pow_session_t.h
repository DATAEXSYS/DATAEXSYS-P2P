#pragma once

#include <stdint.h>
#include <stdbool.h>
// Add necessary includes

typedef struct __attribute__((aligned(4))) {
    tier_pow_challenge_t challenge;
    uint64_t issued_time_seconds;
    uint64_t received_time_seconds;
    uint32_t target_index;
} tier_pow_session_t;
