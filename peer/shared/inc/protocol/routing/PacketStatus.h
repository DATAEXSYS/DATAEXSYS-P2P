#pragma once

#include <stdint.h>

enum PacketStatus : uint16_t
{
     PACKET_STATUS_NONE = 0,

    PACKET_STATUS_DATA,
    PACKET_STATUS_ACK,
    PACKET_STATUS_NACK,
    PACKET_STATUS_NO_PAYLOAD
};