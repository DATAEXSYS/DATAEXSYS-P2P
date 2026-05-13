#pragma once

#include <stdint.h>
#include <stdbool.h>
// Add necessary includes

structure
 * Holds the resulting matrix C (1000 x 1000) where C = A * B.
 * Each entry is a uint32_t to avoid overflow from multiplying two uint16_t's.
 */
typedef struct __attribute__((aligned(4))) {
    uint32_t resultMatrix[MINI_POW_MATRIX_N][MINI_POW_MATRIX_N];
} mini_pow_solve_t;
