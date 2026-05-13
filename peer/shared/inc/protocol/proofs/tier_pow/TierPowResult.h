#pragma once

#include <stdint.h>
#include <stdbool.h>
// Add necessary includes

typedef struct __attribute__((aligned(4))) {
    Tier_t tier;                      
    uint8_t reserved[3];              
    tier_pow_challenge_t challenge;   
    tier_pow_solve_t solve;           
    double time_taken;                
} TierPowResult;
