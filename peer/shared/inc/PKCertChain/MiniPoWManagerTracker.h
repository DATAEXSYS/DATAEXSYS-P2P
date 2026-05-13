#pragma once

#include <stdint.h>
#include <stdbool.h>
// Add necessary includes

typedef struct {
    uint32_t sessionID;
    uint32_t currentIteration;
    MiniPowTracker timeTracker;
} MiniPoWManagerTracker;
