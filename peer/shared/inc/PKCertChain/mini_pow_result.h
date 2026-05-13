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
