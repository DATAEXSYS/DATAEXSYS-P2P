#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "protocol/routing/PacketHeader.h"

#define MAX_PACKET_PAYLOAD 256

typedef struct __attribute__((aligned(4)))
{
    PacketHeader header;

    void* payload;
    uint16_t payload_len;

} RoutingPacket;