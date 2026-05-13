#pragma once
#include <stdint.h>
#include "datatype/uint256_t.h"
#include "datatypes/ipv6.h"

typedef struct __attribute__((aligned(4))) {
    uint256 pubSignKey;      // 32 bytes
    uint256 pubEncKey;       // 32 bytes
    ipv6_t  id;              // 16 bytes
    uint8_t reserved[3];     // 3 bytes
} certificate;
