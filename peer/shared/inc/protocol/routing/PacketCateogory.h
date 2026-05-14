#pragma once

#include <stdint.h>

enum PacketCategory : uint8_t
{
    PACKET_CATEGORY_INVALID = 0,

    PACKET_CATEGORY_CONTROL,   // ping, pong, heartbeat
    PACKET_CATEGORY_ROUTING,   // DSR
    PACKET_CATEGORY_PROOF,     // MiniPoW / TierPoW
    PACKET_CATEGORY_SYNC       // PKC / LTD / RTC
};