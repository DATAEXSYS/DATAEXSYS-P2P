#ifndef SOLVED_MATRIC_POW_H

#include "PKCertChain/SolvedMatricPoW.h"

#define SOLVED_MATRIC_POW_H

#include "pkcertchain_config.h"
#include "util/Size_Offsets.h"
#include <stdint.h>
#include <string.h>
#include "Proofs/MiniPoW/miniPoWClassify.h"

#ifndef SOLVED_MATRIC_POW_INLINE
#define SOLVED_MATRIC_POW_INLINE static inline __attribute__((always_inline))
#endif

/*
 * Final solved matrix returned by the miner along with the puzzle identifiers.
 * Using uint32_t to safely store the sum of the outer products.
 */


SOLVED_MATRIC_POW_INLINE void solved_matric_pow_init(SolvedMatricPoW *solved)
{
    if (!solved) return;
    solved->challenge_id = 0;
    solved->session_id = 0;
    memset(solved->Matrix, 0, sizeof(solved->Matrix));
}

#endif // SOLVED_MATRIC_POW_H
