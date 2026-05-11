// =============================================================================
// PKCertChainAdapter.cpp
//
// All extern "C" calls to the PKCertChain library are confined to this file.
// C struct types (block, certificate, PKCertChain) must not appear in the
// .hpp header.
// =============================================================================

#include "PKCertChainAdapter.hpp"

// All C library includes are local to this translation unit ONLY.
// This is the enforcement boundary.
extern "C" {
#include "util/utilities.h"
#include "blockchain/block.h"
#include "blockchain/pkcertchain.h"
}

namespace dataexsys::adapters {

PKCertChainAdapter::PKCertChainAdapter(AdapterId self_id) noexcept
    : self_id_(self_id) {}

PKCertChainAdapter::~PKCertChainAdapter() { stop(); }

bool PKCertChainAdapter::start()
{
    std::lock_guard lock(mutex_);
    if (running_) return true;
    running_ = true;
    return true;
}

void PKCertChainAdapter::stop()
{
    std::lock_guard lock(mutex_);
    running_ = false;
    outbound_.clear();
}

std::string PKCertChainAdapter::name() const { return "PKCertChainAdapter"; }

AdapterResult PKCertChainAdapter::push(AdapterMessage&& msg)
{
    if (!running_)
        return AdapterResult::fail(AdapterError::ADAPTER_NOT_STARTED);

    switch (msg.opcode) {
        case AdapterOpcode::PKC_NEW_BLOCK:
            return handle_new_block(std::move(msg));
        case AdapterOpcode::PKC_VERIFY_BLOCK:
            return handle_verify_block(std::move(msg));
        default:
            return AdapterResult::fail(AdapterError::UNSUPPORTED_OPCODE);
    }
}

std::optional<AdapterMessage> PKCertChainAdapter::poll()
{
    std::lock_guard lock(mutex_);
    if (outbound_.empty()) return std::nullopt;
    auto msg = std::move(outbound_.front());
    outbound_.pop_front();
    return msg;
}

AdapterResult PKCertChainAdapter::submit_new_block(BufferView serialised_block,
                                                    AdapterId  requester_id)
{
    if (!running_)
        return AdapterResult::fail(AdapterError::ADAPTER_NOT_STARTED);
    if (serialised_block.empty())
        return AdapterResult::fail(AdapterError::PAYLOAD_EMPTY);

    auto msg = AdapterMessage::from_view(
        AdapterOpcode::PKC_NEW_BLOCK,
        requester_id,
        self_id_,
        serialised_block);

    return push(std::move(msg));
}

AdapterResult PKCertChainAdapter::submit_verify_block(BufferView serialised_block,
                                                       AdapterId  requester_id)
{
    if (!running_)
        return AdapterResult::fail(AdapterError::ADAPTER_NOT_STARTED);
    if (serialised_block.empty())
        return AdapterResult::fail(AdapterError::PAYLOAD_EMPTY);

    auto msg = AdapterMessage::from_view(
        AdapterOpcode::PKC_VERIFY_BLOCK,
        requester_id,
        self_id_,
        serialised_block);

    return push(std::move(msg));
}

// ---------------------------------------------------------------------------
// Private — C ABI calls contained here
// ---------------------------------------------------------------------------

AdapterResult PKCertChainAdapter::handle_new_block(AdapterMessage&& msg)
{
    // TODO: Deserialise msg.payload (network byte order) into `block` C struct.
    //       Call PKCertChain_AddBlockWithPoW() or equivalent.
    //       Serialise result back to OwnedBuffer and enqueue_result().
    (void)msg;
    return AdapterResult::fail(AdapterError::TRANSLATION_FAILED,
        "PKCertChainAdapter::handle_new_block — not yet implemented");
}

AdapterResult PKCertChainAdapter::handle_verify_block(AdapterMessage&& msg)
{
    // TODO: Deserialise msg.payload into `block` C struct.
    //       Call verify_tier_pow_solution() or equivalent.
    //       Enqueue result.
    (void)msg;
    return AdapterResult::fail(AdapterError::TRANSLATION_FAILED,
        "PKCertChainAdapter::handle_verify_block — not yet implemented");
}

void PKCertChainAdapter::enqueue_result(AdapterOpcode opcode,
                                         AdapterId     destination,
                                         OwnedBuffer&& result)
{
    std::lock_guard lock(mutex_);
    outbound_.push_back(
        AdapterMessage::make(opcode, self_id_, destination, std::move(result)));
}

} // namespace dataexsys::adapters
