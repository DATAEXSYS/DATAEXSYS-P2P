#pragma once

// =============================================================================
// NetworkAdapter.hpp
//
// Packet ingress/egress boundary adapter.
//
// RESPONSIBILITIES:
//  - Accept raw bytes from the network layer and normalise them into
//    AdapterMessage (NETWORK_PACKET_RECEIVED).
//  - Accept AdapterMessage (NETWORK_PACKET_SEND) and serialise payload
//    to wire format for egress.
//  - Abstract all socket-level framing details.
//
// INVARIANTS:
//  - MUST NOT validate block contents.
//  - MUST NOT perform consensus or trust scoring.
//  - MUST NOT perform HMAC verification (delegate to RollingSignaturesAdapter).
//  - All inbound bytes are treated as opaque until an upstream adapter decides.
// =============================================================================

#if !defined(__linux__)
#error "NetworkAdapter: Linux only."
#endif

#include "Adapter.hpp"

#include <deque>
#include <mutex>
#include <string>

namespace dataexsys::adapters {

class NetworkAdapter final : public IAdapter {
public:
    /// Construct with the adapter's own registered id.
    explicit NetworkAdapter(AdapterId self_id) noexcept;
    ~NetworkAdapter() override;

    // -----------------------------------------------------------------------
    // IAdapter
    // -----------------------------------------------------------------------
    bool                          start()  override;
    void                          stop()   override;
    [[nodiscard]] std::string     name()   const override;
    AdapterResult                 push(AdapterMessage&& msg) override;
    std::optional<AdapterMessage> poll()   override;

    // -----------------------------------------------------------------------
    // NetworkAdapter-specific API
    // -----------------------------------------------------------------------

    /// Ingest raw bytes arriving from a socket.
    /// Frames them, wraps in AdapterMessage(NETWORK_PACKET_RECEIVED),
    /// and enqueues on the outbound queue for upstream adapters to poll.
    AdapterResult ingest_raw(BufferView raw_bytes, AdapterId destination_id);

private:
    AdapterId                  self_id_;
    bool                       running_ = false;

    std::deque<AdapterMessage> outbound_; ///< Messages ready for upstream poll()
    mutable std::mutex         mutex_;

    /// Marshal an AdapterMessage payload into wire-format bytes for socket send.
    /// Returns an empty buffer on failure (caller checks AdapterResult).
    OwnedBuffer marshal_for_wire(const AdapterMessage& msg) const;
};

} // namespace dataexsys::adapters
