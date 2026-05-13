#pragma once

#include <stdint.h>
#include <stdbool.h>
// Add necessary includes


 
typedef struct __attribute__((aligned(4))) {
    bool used;
    mini_pow_challenge_t challenge;
} mini_pow_challenge_queue_entry_t;
