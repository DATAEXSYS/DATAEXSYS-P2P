#pragma once

// =============================================================================
// PKCertChainAdapter.hpp
//
// Sole ABI bridge between nodeEngine and the PKCertChain C library.
//
// RESPONSIBILITIES:
//  - Translate C structs (block, certificate) → opaque byte payloads
//    for AdapterMessage. C types MUST NOT leak past this boundary.
//  - Translate AdapterMessage payloads back to C structs for library calls.
//  - Submit validation jobs to nodeEngine (via a TaskSystem pointer).
//  - Normalise chain events into AdapterMessage.
//
// INVARIANTS:
//  - ONLY this file may contain `extern "C"` for PKCertChain functions.
//  - MUST NOT perform PoW computations directly.
//  - MUST NOT hold blockchain state — delegates to PKCertChain C library.
//  - C struct types from PKCertChain MUST NOT appear in public method
//    signatures of this header.
// =============================================================================

#if !defined(__linux__)
#error "PKCertChainAdapter: Linux only."
#endif

#include "Adapter.hpp"

#include <deque>
#include <mutex>
#include <string>

namespace dataexsys::adapters {

class PKCertChainAdapter final : public IAdapter {
public:
    explicit PKCertChainAdapter(AdapterId self_id) noexcept;
    ~PKCertChainAdapter() override;

    // -----------------------------------------------------------------------
    // IAdapter
    // -----------------------------------------------------------------------
    bool                          start()  override;
    void                          stop()   override;
    [[nodiscard]] std::string     name()   const override;
    AdapterResult                 push(AdapterMessage&& msg) override;
    std::optional<AdapterMessage> poll()   override;

    // -----------------------------------------------------------------------
    // PKCertChainAdapter-specific API
    // -----------------------------------------------------------------------

    /// Submit a new candidate block (serialised payload) for library processing.
    /// Payload must contain the block serialised in network byte order (big-endian).
    AdapterResult submit_new_block(BufferView serialised_block, AdapterId requester_id);

    /// Submit a received block for library verification.
    /// Payload must contain the block serialised in network byte order.
    AdapterResult submit_verify_block(BufferView serialised_block, AdapterId requester_id);

private:
    AdapterId                  self_id_;
    bool                       running_ = false;

    std::deque<AdapterMessage> outbound_;
    mutable std::mutex         mutex_;

    // ------------------------------------------------------------------
    // Internal translation helpers
    // All extern "C" calls are confined to the .cpp implementation.
    // These helpers translate between opaque buffers and C library types.
    // ------------------------------------------------------------------

    /// Deserialise a buffer into C library types and invoke new-block logic.
    AdapterResult handle_new_block(AdapterMessage&& msg);

    /// Deserialise a buffer into C library types and invoke verify-block logic.
    AdapterResult handle_verify_block(AdapterMessage&& msg);

    /// Enqueue a normalised result event back to outbound queue.
    void enqueue_result(AdapterOpcode opcode, AdapterId destination, OwnedBuffer&& result);
};

} // namespace dataexsys::adapters
