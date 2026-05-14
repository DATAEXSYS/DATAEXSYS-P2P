#pragma once

#include <stdint.h>

enum PacketFlags : uint16_t
{
    PACKET_FLAG_NONE        = 0,

    PACKET_FLAG_SIGNED      = 1 << 0,
    PACKET_FLAG_ENCRYPTED   = 1 << 1,
    PACKET_FLAG_HASHED      = 1 << 2,

    PACKET_FLAG_TAMPERED    = 1 << 3,
    PACKET_FLAG_RELAYED     = 1 << 4,
    PACKET_FLAG_FRAGMENTED  = 1 << 5
};