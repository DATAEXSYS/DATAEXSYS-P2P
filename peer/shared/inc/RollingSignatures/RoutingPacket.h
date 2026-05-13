#pragma once

#include <stdint.h>
#include "datatypes/ipv6.h"
#include <stdbool.h>
// Add necessary includes

typedef struct {
    uint8_t payload[256];
    size_t payload_len;
    ipv6_t path_vector[MAX_PATH_HOPS]; // Array of NodeIDs representing the route
    size_t path_len;
    uint8_t hmac[ROUTING_HMAC_SIZE];
    uint32_t nonce;
} RoutingPacket;
