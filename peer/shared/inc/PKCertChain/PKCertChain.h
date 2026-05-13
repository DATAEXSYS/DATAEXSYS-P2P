#pragma once

#include <stdint.h>
#include <stdbool.h>
// Add necessary includes

typedef struct __attribute__((aligned(4))) {
    block blocks[100]; //148
    uint32_t index;
    char NetworkName[64];
    uint8_t complexity;
    uint64_t next_challenge_id;
    double avg_solve_time_seconds;
    uint32_t lastMCUBlockIndex;
    uint32_t lastServerBlockIndex;
    uint32_t lastDesktopBlockIndex;
    uint32_t lastEdgeBlockIndex;

    uint8_t MCUComplexity;
    uint8_t ServerComplexity;
    uint8_t DesktopComplexity;
    uint8_t EdgeComplexity;
} PKCertChain;
