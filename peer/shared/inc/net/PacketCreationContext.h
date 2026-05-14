#pragma once

#include "protocol/routing/PacketCategory.h"
#include "protocol/routing/PacketOperation.h"
#include "protocol/routing/PacketStatus.h"
#include "core/datatypes/ipv6.h"

struct PacketCreationContext
{
    PacketCategory category;
    PacketOperation operation;
    PacketStatus status;

    ipv6_t sender;
    ipv6_t destination;

    const void* payload;
    uint16_t payload_len;
    
    //uint32_t nonce;
};