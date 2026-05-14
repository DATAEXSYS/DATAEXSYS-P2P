#pragma once

#include <stdint.h>

enum PacketFlags : uint32_t
{
    PACKET_FLAG_NONE = 0,

    // =================================================
    // SECURITY LAYER
    // =================================================

    PACKET_FLAG_SIGNED             = (1 << 0),
    PACKET_FLAG_HASHED             = (1 << 1),
    PACKET_FLAG_ENCRYPTED          = (1 << 2),

    // verification state (runtime only)
    PACKET_FLAG_VERIFIED          = (1 << 3),
    PACKET_FLAG_TAMPERED          = (1 << 4),

    // =================================================
    // DELIVERY BEHAVIOR
    // =================================================

    // expects response (ACK at protocol level)
    PACKET_FLAG_ACK_REQUIRED      = (1 << 5),

    // retransmission tracking
    PACKET_FLAG_RETRANSMITTED     = (1 << 6),

    // =================================================
    // ROUTING BEHAVIOR
    // =================================================

    PACKET_FLAG_FORWARDABLE       = (1 << 7),
    PACKET_FLAG_FINAL_DESTINATION = (1 << 8),
    PACKET_FLAG_DROP_AFTER_READ   = (1 << 9),
    PACKET_FLAG_BROADCAST         = (1 << 10),
    PACKET_FLAG_MULTICAST         = (1 << 11),

    // =================================================
    // PRIORITY / TIMING
    // =================================================

    PACKET_FLAG_PRIORITY_HIGH     = (1 << 12),
    PACKET_FLAG_REALTIME          = (1 << 13),

    // =================================================
    // SESSION / SYNC
    // =================================================

    PACKET_FLAG_SESSION           = (1 << 14),
    PACKET_FLAG_SYNC              = (1 << 15),

    // =================================================
    // CONTENT TYPE HINTS
    // =================================================

    PACKET_FLAG_CHALLENGE         = (1 << 16),
    PACKET_FLAG_RESULT            = (1 << 17),

    // =================================================
    // ROUTING PROTOCOL FLAGS
    // =================================================

    PACKET_FLAG_ROUTE_DISCOVERY   = (1 << 18),
    PACKET_FLAG_ROUTE_REPLY       = (1 << 19),
    PACKET_FLAG_ROUTE_ERROR       = (1 << 20),

    // =================================================
    // PROOF SYSTEM FLAGS
    // =================================================

    PACKET_FLAG_MINIPOW           = (1 << 21),
    PACKET_FLAG_TIERPOW           = (1 << 22),

    // =================================================
    // RUNTIME ENGINE STATE
    // =================================================

    PACKET_FLAG_PROCESSED         = (1 << 23),
    PACKET_FLAG_QUEUED            = (1 << 24),
    PACKET_FLAG_EXECUTING         = (1 << 25),
    PACKET_FLAG_EXPIRED           = (1 << 26)
};