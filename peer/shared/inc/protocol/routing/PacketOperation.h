#pragma once

#include <stdint.h>

enum PacketOperation : uint16_t
{
    PACKET_OP_INVALID = 0,

    // =================================================
    // DSR
    // =================================================
    PACKET_OP_DSR_RREQ,
    PACKET_OP_DSR_RREP,
    PACKET_OP_DSR_RERR,
    PACKET_OP_DSR_DATA,

    // =================================================
    // MINI POW
    // =================================================
    PACKET_OP_MINIPOW_CHALLENGE,
    PACKET_OP_MINIPOW_RESULT,

    // =================================================
    // TIER POW
    // =================================================
    PACKET_OP_TIERPOW_CHALLENGE,
    PACKET_OP_TIERPOW_RESULT,

    // =================================================
    // SYNC
    // =================================================
    PACKET_OP_PKC_SYNC,
    PACKET_OP_LTD_SYNC,
    PACKET_OP_RTC_SYNC,

    // =================================================
    // CONTROL
    // =================================================
    PACKET_OP_PING,
    PACKET_OP_PONG,
    PACKET_OP_HEARTBEAT
};