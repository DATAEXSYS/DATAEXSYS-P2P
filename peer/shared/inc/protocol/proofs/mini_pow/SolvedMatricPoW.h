#pragma once

#include <stdint.h>
#include <stdbool.h>
// Add necessary includes

typedef struct __attribute__((aligned(4))) {
    uint32_t challenge_id;
    uint32_t session_id;
    uint32_t Matrix[MINI_POW_MATRIX_N][MINI_POW_MATRIX_N];
} SolvedMatricPoW;
