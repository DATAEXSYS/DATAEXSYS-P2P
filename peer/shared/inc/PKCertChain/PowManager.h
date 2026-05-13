#pragma once

#include <stdint.h>
#include <stdbool.h>
// Add necessary includes

typedef struct {
    PKCertChain *chain;
    uint8_t tier;
    MiniPowResult *miniResult;
    tier_pow_challenge_t challenge;
    tier_pow_solve_t solve;
    double solve_time_seconds;
} PowManager;
