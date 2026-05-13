#pragma once

#include <stdint.h>
#include <stdbool.h>
// Add necessary includes

typedef struct {
    uint32_t challengeid;
    uint32_t sessionid;
    const mini_pow_Matrix *minipowmatrix;
    const SolvedMatricPoW *solvedmatrix;
    Tier_t tier;
    bool isValid;
} mini_pow_result;

typedef struct __attribute__((aligned(4))) {
    uint32_t challengeid; // 4
    uint32_t sessionid;   // 4
    Tier_t tier;          // 1
    bool isValid;         // 1
    uint8_t reserved[2];  // 2 -> Total 12
} MiniPowResult;
