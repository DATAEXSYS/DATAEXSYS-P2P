#pragma once

// =============================================================================
// LocalTrustDiariesAdapter.hpp
//
// Bridge to the LocalTrustDiaries header-only C++23 library.
//
// RESPONSIBILITIES:
//  - Trust lookup by node ID.
//  - Trust mutation (ACK / NACK via AdapterMessage).
//  - Trust snapshot generation for frontend or routing consumers.
//  - Scheduling hooks for future trust decay.
//
// INVARIANTS:
//  - nodeEngine MUST NEVER touch TrustNode directly.
//  - TrustNode and LocalTrustDiaries types MUST NOT appear in this header.
//  - trust_node_update_trust() body is currently empty in the C++ library;
//    this adapter will call it when implemented, but MUST NOT implement the
//    scoring logic itself.
// =============================================================================

#if !defined(__linux__)
#error "LocalTrustDiariesAdapter: Linux only."
#endif

#include "Adapter.hpp"

#include <cstdint>
#include <deque>
#include <mutex>
#include <string>
#include <vector>

namespace dataexsys::adapters {

/// Opaque trust snapshot — serialised summary of a peer's trust state.
/// Format: [nodeid(1)] [trust(4 LE)] [acks(4 LE)] [nacks(4 LE)] = 13 bytes.
struct TrustSnapshot {
    uint8_t  node_id;
    uint32_t trust;
    uint32_t acks;
    uint32_t nacks;

    /// Serialise to OwnedBuffer in the format above.
    [[nodiscard]] OwnedBuffer serialise() const;

    /// Deserialise from a BufferView. Returns nullopt on malformed input.
    [[nodiscard]] static std::optional<TrustSnapshot> deserialise(BufferView view);
};

class LocalTrustDiariesAdapter final : public IAdapter {
public:
    explicit LocalTrustDiariesAdapter(AdapterId self_id,
                                       std::size_t max_peers = 256) noexcept;
    ~LocalTrustDiariesAdapter() override;

    // -----------------------------------------------------------------------
    // IAdapter
    // -----------------------------------------------------------------------
    bool                          start()  override;
    void                          stop()   override;
    [[nodiscard]] std::string     name()   const override;
    AdapterResult                 push(AdapterMessage&& msg) override;
    std::optional<AdapterMessage> poll()   override;

    // -----------------------------------------------------------------------
    // LocalTrustDiariesAdapter-specific API
    // -----------------------------------------------------------------------

    /// Look up the trust snapshot for a given node id.
    /// Returns nullopt if node_id is not in the ledger.
    [[nodiscard]]
    std::optional<TrustSnapshot> lookup(uint8_t node_id) const;

    /// Record a positive interaction with node_id.
    AdapterResult record_ack(uint8_t node_id);

    /// Record a negative interaction with node_id.
    AdapterResult record_nack(uint8_t node_id);

    /// Generate a serialised snapshot of all known peers.
    /// Enqueues as FRONTEND_STATUS_UPDATE for the FrontendAdapter to poll.
    AdapterResult broadcast_trust_snapshot(AdapterId destination_id);

private:
    AdapterId                  self_id_;
    std::size_t                max_peers_;
    bool                       running_ = false;

    // Internal ledger — TrustNode from LocalTrustDiaries, hidden in .cpp
    // so TrustNode never leaks into this header.
    struct Impl;
    Impl*                      impl_ = nullptr;  ///< Pimpl for C++ isolation

    std::deque<AdapterMessage> outbound_;
    mutable std::mutex         mutex_;

    AdapterResult handle_ack(AdapterMessage&& msg);
    AdapterResult handle_nack(AdapterMessage&& msg);
};

} // namespace dataexsys::adapters
