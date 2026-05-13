#pragma once

#include <stdint.h>
#include "protocol/proofs/mini_pow/mini_pow_Matrix.h"
#include <stdbool.h>
// Add necessary includes

typedef struct __attribute__((aligned(4))) {
    uint32_t challenge_id;
    uint32_t session_id;
    uint32_t iteration;
    uint16_t columnOfA[MINI_POW_MATRIX_N];
    uint16_t rowOfB[MINI_POW_MATRIX_N];
} mini_pow_challenge_t;
