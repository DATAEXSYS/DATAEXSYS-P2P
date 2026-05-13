#pragma once

#include <stdint.h>
#include <stdbool.h>
// Add necessary includes

typedef struct __attribute__((aligned(4))) {
    uint256 seed;
    uint16_t A[MINI_POW_MATRIX_N][MINI_POW_MATRIX_N];
    uint16_t B[MINI_POW_MATRIX_N][MINI_POW_MATRIX_N];
} mini_pow_Matrix;
