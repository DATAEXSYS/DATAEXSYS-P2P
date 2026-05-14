#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "protocol/routing/PacketCategory.h"
#include "protocol/routing/PacketOperation.h"
#include "protocol/routing/PacketStatus.h"
#include "protocol/routing/PacketFlags.h"
#include "core/datatypes/ipv6.h"

#define MAX_PATH_HOPS 16

typedef struct __attribute__((aligned(4)))
{
    uint32_t packet_id;

    PacketCategory category;
    PacketOperation operation;
    PacketStatus status;
    PacketFlags flags;

    ipv6_t sender;
    ipv6_t destination;

    ipv6_t path_vector[MAX_PATH_HOPS];
    uint8_t path_len;

    uint8_t ttl;

    uint32_t nonce;

    // STATIC AUTH (end-to-end)
    uint8_t signature[64];  //signature = Sign(sender_private, hash(full packet))

    // DYNAMIC AUTH (hop-by-hop rolling HMAC)
    uint8_t rolling_hmac[32];

} PacketHeader;