#ifndef MINI_POW_CHALLENGE_H

#include "PKCertChain/mini_pow_challenge_t.h"

#define MINI_POW_CHALLENGE_H

#include "pkcertchain_config.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "datatype/OpStatus.h"
#include "Proofs/MiniPoW/miniPoWClassify.h"
#include "util/Size_Offsets.h"

#define MINI_POW_CHALLENGE_INLINE static inline __attribute__((always_inline))

/*
 * MiniPoW Challenge 


MINI_POW_CHALLENGE_INLINE void mini_pow_challenge_init(mini_pow_challenge_t *pow)
{
    if (!pow) return;
    pow->challenge_id = 0;
    pow->session_id = 0;
    pow->iteration = 0;
    memset(pow->columnOfA, 0, sizeof(pow->columnOfA));
    memset(pow->rowOfB, 0, sizeof(pow->rowOfB));
}

#endif // MINI_POW_CHALLENGE_H
