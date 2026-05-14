#pragma once

#include <stdint.h>

// =====================================================
// PACKET FLAGS
// =====================================================
//
// These flags describe HOW a packet should be treated,
// verified, routed, retransmitted, or acknowledged.
//
// They are transport/runtime semantics,
// NOT payload semantics.
//
// Can be OR-ed together.
//
// Example:
// flags = PACKET_FLAG_SIGNED |
//         PACKET_FLAG_ACK_REQUIRED |
//         PACKET_FLAG_FORWARDABLE;
//
// =====================================================

enum PacketFlags : uint32_t
{
    PACKET_FLAG_NONE                = 0,

    // =================================================
    // SECURITY FLAGS
    // =================================================

    // Payload contains cryptographic signature
    PACKET_FLAG_SIGNED             = (1 << 0),

    // Payload contains hash/HMAC
    PACKET_FLAG_HASHED             = (1 << 1),

    // Payload encrypted
    PACKET_FLAG_ENCRYPTED          = (1 << 2),

    // Packet integrity already verified
    PACKET_FLAG_VERIFIED           = (1 << 3),

    // Packet failed integrity/security checks
    PACKET_FLAG_TAMPERED           = (1 << 4),

    // =================================================
    // DELIVERY FLAGS
    // =================================================

    // Sender expects ACK
    PACKET_FLAG_ACK_REQUIRED       = (1 << 5),

    // Packet is ACK response
    PACKET_FLAG_IS_ACK             = (1 << 6),

    // Packet is NACK response
    PACKET_FLAG_IS_NACK            = (1 << 7),

    // =================================================
    // ROUTING FLAGS
    // =================================================

    // Packet may be forwarded
    PACKET_FLAG_FORWARDABLE        = (1 << 8),

    // Final destination reached
    PACKET_FLAG_FINAL_DESTINATION  = (1 << 9),

    // Packet should not be forwarded further
    PACKET_FLAG_DROP_AFTER_READ    = (1 << 10),

    // Broadcast packet
    PACKET_FLAG_BROADCAST          = (1 << 11),

    // Multicast packet
    PACKET_FLAG_MULTICAST          = (1 << 12),

    // =================================================
    // RELIABILITY FLAGS
    // =================================================

    // High priority packet
    PACKET_FLAG_PRIORITY_HIGH      = (1 << 13),

    // Real-time sensitive
    PACKET_FLAG_REALTIME           = (1 << 14),

    // Retransmitted packet
    PACKET_FLAG_RETRANSMITTED      = (1 << 15),

    // Fragmented packet
    PACKET_FLAG_FRAGMENT           = (1 << 16),

    // =================================================
    // SESSION / STATE FLAGS
    // =================================================

    // Packet belongs to active session
    PACKET_FLAG_SESSION            = (1 << 17),

    // Stateful synchronization packet
    PACKET_FLAG_SYNC               = (1 << 18),

    // Packet contains challenge
    PACKET_FLAG_CHALLENGE          = (1 << 19),

    // Packet contains result
    PACKET_FLAG_RESULT             = (1 << 20),

    // =================================================
    // DSR / ROUTE FLAGS
    // =================================================

    // Route discovery packet
    PACKET_FLAG_ROUTE_DISCOVERY    = (1 << 21),

    // Route reply packet
    PACKET_FLAG_ROUTE_REPLY        = (1 << 22),

    // Route error packet
    PACKET_FLAG_ROUTE_ERROR        = (1 << 23),

    // =================================================
    // POW FLAGS
    // =================================================

    // MiniPoW-related packet
    PACKET_FLAG_MINIPOW            = (1 << 24),

    // TierPoW-related packet
    PACKET_FLAG_TIERPOW            = (1 << 25),

    // =================================================
    // INTERNAL RUNTIME FLAGS
    // =================================================

    // Packet already processed
    PACKET_FLAG_PROCESSED          = (1 << 26),

    // Packet queued
    PACKET_FLAG_QUEUED             = (1 << 27),

    // Packet currently executing
    PACKET_FLAG_EXECUTING          = (1 << 28),

    // Packet expired (TTL timeout)
    PACKET_FLAG_EXPIRED            = (1 << 29)
};