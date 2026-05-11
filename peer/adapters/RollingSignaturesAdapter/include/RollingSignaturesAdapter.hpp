#pragma once

// =============================================================================
// RollingSignaturesAdapter.hpp
//
// Sole ABI bridge between nodeEngine and the RollingSignatures C library.
//
// RESPONSIBILITIES:
//  - Wrap `verify_packet()` and `forward_packet()` from hmac_routing.h.
//  - Maintain per-session replay-window state (nonce tracking).
//  - Translate results into AdapterMessage for upstream routing decisions.
//
// INVARIANTS:
//  - MUST NOT implement any crypto primitives.
//  - MUST NOT derive keys — only delegates to the C library.
//  - ONLY this file may contain `extern "C"` for RollingSignatures functions.
//  - `RoutingPacket` and `hmac_routing.h` types MUST NOT appear in this header.
// =============================================================================

#if !defined(__linux__)
#error "RollingSignaturesAdapter: Linux only."
#endif

#include "Adapter.hpp"

#include <deque>
#include <mutex>
#include <string>
#include <unordered_set>
#include <cstdint>

namespace dataexsys::adapters {

class RollingSignaturesAdapter final : public IAdapter {
public:
    explicit RollingSignaturesAdapter(AdapterId self_id) noexcept;
    ~RollingSignaturesAdapter() override;

    // -----------------------------------------------------------------------
    // IAdapter
    // -----------------------------------------------------------------------
    bool                          start()  override;
    void                          stop()   override;
    [[nodiscard]] std::string     name()   const override;
    AdapterResult                 push(AdapterMessage&& msg) override;
    std::optional<AdapterMessage> poll()   override;

    // -----------------------------------------------------------------------
    // RollingSignaturesAdapter-specific API
    // -----------------------------------------------------------------------

    /// Verify an inbound packet's HMAC chain.
    /// Payload must be the serialised RoutingPacket in wire format.
    /// Returns success if verify_packet() returns true; fails with
    /// TRANSLATION_FAILED if the wire bytes cannot be parsed.
    AdapterResult submit_verify(BufferView wire_packet, AdapterId requester_id);

    /// Forward a packet: verify → append node-id → re-HMAC.
    /// Payload must contain: [wire_packet | local_node_id (1 byte)].
    /// The updated packet is placed in the outbound queue as a new message.
    AdapterResult submit_forward(BufferView wire_packet,
                                 uint8_t    local_node_id,
                                 AdapterId  requester_id);

private:
    AdapterId                     self_id_;
    bool                          running_ = false;

    std::deque<AdapterMessage>    outbound_;
    mutable std::mutex            mutex_;

    /// Seen nonces — simplistic replay window.
    /// Future: replace with a sliding-window bitset bounded by sequence number.
    std::unordered_set<uint64_t>  seen_nonces_;
    mutable std::mutex            nonce_mutex_;

    AdapterResult handle_verify(AdapterMessage&& msg);
    AdapterResult handle_forward(AdapterMessage&& msg);

    /// Return true if the nonce has been seen before (replay detected).
    /// Registers the nonce if not seen.
    bool is_replay(uint64_t nonce);
};

} // namespace dataexsys::adapters
