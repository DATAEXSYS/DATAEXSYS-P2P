#pragma once

#include <stdint.h>
#include <stdbool.h>
// Add necessary includes

typedef struct __attribute__((aligned(4))) {
    tier_pow_queue_entry_t entries[TIER_POW_QUEUE_MAX];
    size_t count;
} tier_pow_queue_t;
