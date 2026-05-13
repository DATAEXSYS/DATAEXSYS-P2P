#pragma once

#include <stdint.h>
#include <stdbool.h>
// Add necessary includes

typedef struct __attribute__((aligned(4))) {
    mini_pow_challenge_queue_entry_t entries[MINI_POW_CHALLENGE_RECEIVE_QUEUE_MAX];
    size_t count;
} mini_pow_challenge_receive_queue_t;
