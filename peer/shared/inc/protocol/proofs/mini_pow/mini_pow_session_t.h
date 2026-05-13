#pragma once

#include <stdint.h>
#include <stdbool.h>
// Add necessary includes

typedef struct __attribute__((aligned(4))) {
    mini_pow_challenge_t challenge;
    uint64_t issued_time_seconds;
    uint64_t received_time_seconds;
    uint32_t target_index;
} mini_pow_session_t;
