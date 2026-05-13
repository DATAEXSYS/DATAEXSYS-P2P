#pragma once

#include <stdint.h>
#include <stdbool.h>
// Add necessary includes

typedef struct __attribute__((aligned(4))) {
    uint32_t resultMatrix[MINI_POW_MATRIX_N][MINI_POW_MATRIX_N];
} mini_pow_solve_t;
