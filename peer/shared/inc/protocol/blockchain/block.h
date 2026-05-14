#pragma once

#include <stdint.h>
#include "core/datatypes/uint256_t.h"
#include "core/datatypes/uint512.h"
#include "protocol/proofs/mini_pow/mini_pow_result.h"
#include "protocol/proofs/tier_pow/TierPowResult.h"
#include <stdbool.h>
#include "protocol/blockchain/certificate.h"
#include "core/enums/Tier.h"
// Add necessary includes

typedef struct __attribute__((aligned(4)))
{
    certificate cert; // 68 bytes: pubSignKey + pubEncKey + id
    uint256 CurrentCertHash; // 32 bytes: hash of certificate
    uint256 prevHash; //32
    uint512 SignedByVerifier; // 64 bytes signature
    uint64_t height; //8
    uint64_t timestamp;   // 8 monotonic time, canonical 64-bit
    Tier_t tier; // 1 byte
    uint8_t reserved[3]; // padding
    MiniPowResult miniPowResult;
    TierPowResult tierPoWResult;
} block;
