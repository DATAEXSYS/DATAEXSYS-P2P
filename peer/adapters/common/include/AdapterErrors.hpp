#pragma once

// =============================================================================
// AdapterErrors.hpp
//
// Strongly typed error codes for AdapterResult.
// RULE: All adapter error paths must produce an AdapterError — never a raw int
//       or a string-only diagnosis.
// =============================================================================

#include <cstdint>

namespace dataexsys::adapters {

enum class AdapterError : uint16_t {

    NONE = 0,

    // Payload / buffer errors
    PAYLOAD_EMPTY         = 0x0001,  ///< Message carries zero-length payload
    PAYLOAD_TOO_LARGE     = 0x0002,  ///< Payload exceeds adapter-specific limit
    NULL_BUFFER           = 0x0003,  ///< Caller passed a null or invalid span

    // Opcode errors
    INVALID_OPCODE        = 0x0010,  ///< Opcode is 0x0000 or out of range
    UNSUPPORTED_OPCODE    = 0x0011,  ///< This adapter does not handle that opcode

    // Routing errors
    INVALID_SOURCE        = 0x0020,  ///< source_id is not a registered adapter
    INVALID_DESTINATION   = 0x0021,  ///< destination_id is not a registered adapter

    // Lifecycle errors
    ADAPTER_NOT_STARTED   = 0x0030,  ///< push()/poll() called before start()
    ADAPTER_ALREADY_STARTED = 0x0031,///< start() called on a running adapter
    LIFECYCLE_VIOLATION   = 0x0032,  ///< Illegal state transition

    // Queue / capacity errors
    QUEUE_FULL            = 0x0040,  ///< Internal message queue is at capacity

    // Translation / marshalling errors
    TRANSLATION_FAILED    = 0x0050,  ///< C struct ↔ C++ type conversion failed
    SERIALISATION_FAILED  = 0x0051,  ///< Buffer → wire format conversion failed
    DESERIALISATION_FAILED = 0x0052, ///< Wire bytes → typed struct conversion failed
};

} // namespace dataexsys::adapters
