#pragma once

// =============================================================================
// AdapterMessage.hpp
//
// Universal message envelope passed between all adapters.
//
// RULES:
//  - nodeEngine MUST interact with backend subsystems ONLY via AdapterMessage.
//  - Backend C structs MUST NOT appear outside the adapter that owns them.
//  - AdapterMessage carries only opaque byte payloads — no typed C structs.
//  - source_id / destination_id must refer to registered AdapterIds.
// =============================================================================

#include "AdapterOpcode.hpp"
#include "BufferView.hpp"

#include <chrono>
#include <cstdint>

namespace dataexsys::adapters {

/// Numeric identifier for a registered adapter.
/// Values are assigned by the adapter registry (future component).
using AdapterId = uint16_t;

constexpr AdapterId ADAPTER_ID_UNSET = 0x0000;

// ---------------------------------------------------------------------------
// AdapterMessage
// ---------------------------------------------------------------------------

struct AdapterMessage {

    AdapterOpcode                         opcode;
    AdapterId                             source_id;
    AdapterId                             destination_id;
    std::chrono::steady_clock::time_point timestamp;
    OwnedBuffer                           payload;

    // -----------------------------------------------------------------------
    // Factory methods (the only construction paths)
    // -----------------------------------------------------------------------

    /// Create a message with an already-owned payload buffer (move-in).
    [[nodiscard]]
    static AdapterMessage make(
        AdapterOpcode opcode,
        AdapterId     source_id,
        AdapterId     destination_id,
        OwnedBuffer&& payload) noexcept;

    /// Create a message by copying bytes from an immutable view.
    /// Use when the source buffer has shorter lifetime than the message.
    [[nodiscard]]
    static AdapterMessage from_view(
        AdapterOpcode opcode,
        AdapterId     source_id,
        AdapterId     destination_id,
        BufferView    view);

    // -----------------------------------------------------------------------
    // Accessors
    // -----------------------------------------------------------------------

    /// Zero-copy read-only view into the payload bytes.
    [[nodiscard]]
    BufferView payload_view() const noexcept
    {
        return make_view(payload);
    }

    /// True if the message carries at least one byte of payload.
    [[nodiscard]]
    bool has_payload() const noexcept { return !payload.empty(); }

    // -----------------------------------------------------------------------
    // Deleted constructors — use factory methods only
    // -----------------------------------------------------------------------
    AdapterMessage() = delete;

private:
    // Private constructor used exclusively by the static factory methods.
    AdapterMessage(
        AdapterOpcode                         opcode,
        AdapterId                             source_id,
        AdapterId                             destination_id,
        std::chrono::steady_clock::time_point timestamp,
        OwnedBuffer&&                         payload) noexcept
        : opcode(opcode)
        , source_id(source_id)
        , destination_id(destination_id)
        , timestamp(timestamp)
        , payload(std::move(payload))
    {}
};

} // namespace dataexsys::adapters
