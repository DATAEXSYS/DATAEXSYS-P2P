#pragma once

#include <stdint.h>

enum PacketType : uint16_t
{
    PACKET_INVALID = 0,

    // =================================================
    // DSR ROUTING
    // =================================================
    PACKET_DSR_RREQ,
    PACKET_DSR_RREP,
    PACKET_DSR_RERR,
    PACKET_DSR_DATA,
    PACKET_DSR_ACK,
    PACKET_DSR_NACK,

    // =================================================
    // MINI POW
    // =================================================
    PACKET_MINIPOW_CHALLENGE,
    PACKET_MINIPOW_ACK,
    PACKET_MINIPOW_RESULT,

    // =================================================
    // TIER POW
    // =================================================
    PACKET_TIERPOW_CHALLENGE,
    PACKET_TIERPOW_RESULT,

    // =================================================
    // STATE SYNC
    // =================================================
    PACKET_PKC_SYNC,
    PACKET_LTD_SYNC,
    PACKET_RTC_SYNC,

    // =================================================
    // CONTROL
    // =================================================
    PACKET_PING,
    PACKET_PONG,
    PACKET_HEARTBEAT
};