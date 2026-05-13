#pragma once

#include <stdint.h>
#include <stdbool.h>
// Add necessary includes

typedef struct __attribute__((aligned(4))) {
    uint32_t challenge_id;
    uint32_t session_id;
    uint64_t recent_start_time;
    uint64_t recent_receive_time;
    uint64_t cumulative_duration;
} MiniPowTracker;
