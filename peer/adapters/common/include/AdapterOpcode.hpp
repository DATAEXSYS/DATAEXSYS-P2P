#pragma once

// =============================================================================
// AdapterOpcode.hpp
//
// Strongly typed opcode enum for all inter-adapter messages.
// Range layout:
//   0x0000         — reserved / unknown
//   0x0001–0x00FF  — Network subsystem
//   0x0100–0x01FF  — PKCertChain subsystem
//   0x0200–0x02FF  — RollingSignatures subsystem
//   0x0300–0x03FF  — Trust subsystem (LocalTrustDiaries)
//   0x0400–0x04FF  — Frontend subsystem
//   0x0500–0xFFFF  — reserved for future subsystems
//
// RULE: Do not add opcodes that encode business logic.
//       An opcode names a transfer event, not a command to perform crypto.
// =============================================================================

#include <cstdint>

namespace dataexsys::adapters {

enum class AdapterOpcode : uint16_t {

    UNKNOWN = 0x0000,

    // -----------------------------------------------------------------------
    // Network subsystem
    // -----------------------------------------------------------------------
    NETWORK_PACKET_RECEIVED  = 0x0001,  ///< Raw bytes arrived from socket
    NETWORK_PACKET_SEND      = 0x0002,  ///< Serialised payload queued for egress

    // -----------------------------------------------------------------------
    // PKCertChain subsystem
    // -----------------------------------------------------------------------
    PKC_NEW_BLOCK            = 0x0101,  ///< A new candidate block is ready
    PKC_VERIFY_BLOCK         = 0x0102,  ///< Request verification of a received block

    // -----------------------------------------------------------------------
    // RollingSignatures subsystem
    // -----------------------------------------------------------------------
    ROLLING_VERIFY_PACKET    = 0x0201,  ///< Verify HMAC chain on an inbound packet
    ROLLING_FORWARD_PACKET   = 0x0202,  ///< Append node ID + re-HMAC and forward

    // -----------------------------------------------------------------------
    // Trust subsystem (LocalTrustDiaries)
    // -----------------------------------------------------------------------
    TRUST_ACK                = 0x0301,  ///< Positive interaction — increment acks
    TRUST_NACK               = 0x0302,  ///< Negative interaction — increment nacks

    // -----------------------------------------------------------------------
    // Frontend subsystem
    // -----------------------------------------------------------------------
    FRONTEND_STATUS_UPDATE   = 0x0401,  ///< Push node status snapshot to frontend
};

} // namespace dataexsys::adapters
