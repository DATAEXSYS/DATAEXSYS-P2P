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

    uint8_t ttl;
    uint8_t path_len;

    uint32_t nonce;

    uint16_t payload_length;

} PacketHeader;