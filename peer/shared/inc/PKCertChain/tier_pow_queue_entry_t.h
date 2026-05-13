#pragma once

#include <stdint.h>
#include <stdbool.h>
// Add necessary includes

typedef struct __attribute__((aligned(4))) {
    bool used;
    tier_pow_session_t session;
    block candidate;
} tier_pow_queue_entry_t;
