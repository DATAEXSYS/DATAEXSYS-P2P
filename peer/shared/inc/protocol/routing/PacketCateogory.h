#pragma once

#include <stdint.h>

enum PacketCategory : uint8_t
{
    PACKET_CATEGORY_INVALID = 0,

    PACKET_CATEGORY_REQUEST,
    PACKET_CATEGORY_ACK,
    PACKET_CATEGORY_NACK
};