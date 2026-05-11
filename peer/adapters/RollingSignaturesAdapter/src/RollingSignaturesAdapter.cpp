// =============================================================================
// RollingSignaturesAdapter.cpp
//
// All extern "C" calls to the RollingSignatures library are confined here.
// RoutingPacket and hmac_routing.h types must NOT leak to the .hpp header.
// =============================================================================

#include "RollingSignaturesAdapter.hpp"

extern "C" {
#include "hmac_routing.h"
}

namespace dataexsys::adapters {

RollingSignaturesAdapter::RollingSignaturesAdapter(AdapterId self_id) noexcept
    : self_id_(self_id) {}

RollingSignaturesAdapter::~RollingSignaturesAdapter() { stop(); }

bool RollingSignaturesAdapter::start()
{
    std::lock_guard lock(mutex_);
    if (running_) return true;
    running_ = true;
    return true;
}

void RollingSignaturesAdapter::stop()
{
    std::lock_guard lock(mutex_);
    running_ = false;
    outbound_.clear();
    std::lock_guard nonce_lock(nonce_mutex_);
    seen_nonces_.clear();
}

std::string RollingSignaturesAdapter::name() const
{
    return "RollingSignaturesAdapter";
}

AdapterResult RollingSignaturesAdapter::push(AdapterMessage&& msg)
{
    if (!running_)
        return AdapterResult::fail(AdapterError::ADAPTER_NOT_STARTED);

    switch (msg.opcode) {
        case AdapterOpcode::ROLLING_VERIFY_PACKET:
            return handle_verify(std::move(msg));
        case AdapterOpcode::ROLLING_FORWARD_PACKET:
            return handle_forward(std::move(msg));
        default:
            return AdapterResult::fail(AdapterError::UNSUPPORTED_OPCODE);
    }
}

std::optional<AdapterMessage> RollingSignaturesAdapter::poll()
{
    std::lock_guard lock(mutex_);
    if (outbound_.empty()) return std::nullopt;
    auto msg = std::move(outbound_.front());
    outbound_.pop_front();
    return msg;
}

AdapterResult RollingSignaturesAdapter::submit_verify(BufferView wire_packet,
                                                       AdapterId  requester_id)
{
    if (!running_)
        return AdapterResult::fail(AdapterError::ADAPTER_NOT_STARTED);
    if (wire_packet.empty())
        return AdapterResult::fail(AdapterError::PAYLOAD_EMPTY);

    auto msg = AdapterMessage::from_view(
        AdapterOpcode::ROLLING_VERIFY_PACKET,
        requester_id, self_id_, wire_packet);

    return push(std::move(msg));
}

AdapterResult RollingSignaturesAdapter::submit_forward(BufferView wire_packet,
                                                        uint8_t    local_node_id,
                                                        AdapterId  requester_id)
{
    if (!running_)
        return AdapterResult::fail(AdapterError::ADAPTER_NOT_STARTED);
    if (wire_packet.empty())
        return AdapterResult::fail(AdapterError::PAYLOAD_EMPTY);

    // Pack: [wire_packet bytes ... | local_node_id]
    OwnedBuffer payload(wire_packet.begin(), wire_packet.end());
    payload.push_back(local_node_id);

    auto msg = AdapterMessage::make(
        AdapterOpcode::ROLLING_FORWARD_PACKET,
        requester_id, self_id_, std::move(payload));

    return push(std::move(msg));
}

// ---------------------------------------------------------------------------
// Private — C library calls confined here
// ---------------------------------------------------------------------------

AdapterResult RollingSignaturesAdapter::handle_verify(AdapterMessage&& msg)
{
    // TODO: Deserialise msg.payload → RoutingPacket.
    //       Extract nonce, call is_replay(nonce).
    //       Call verify_packet(pkt, k0) using the session key.
    //       Push result message to outbound.
    (void)msg;
    return AdapterResult::fail(AdapterError::TRANSLATION_FAILED,
        "RollingSignaturesAdapter::handle_verify — not yet implemented");
}

AdapterResult RollingSignaturesAdapter::handle_forward(AdapterMessage&& msg)
{
    // TODO: Split payload back into [wire_packet | node_id].
    //       Call forward_packet(pkt, node_id, k_prev, k_out).
    //       Re-serialise updated packet → push to outbound.
    (void)msg;
    return AdapterResult::fail(AdapterError::TRANSLATION_FAILED,
        "RollingSignaturesAdapter::handle_forward — not yet implemented");
}

bool RollingSignaturesAdapter::is_replay(uint64_t nonce)
{
    std::lock_guard lock(nonce_mutex_);
    auto [it, inserted] = seen_nonces_.insert(nonce);
    return !inserted;  // true = already seen → replay
}

} // namespace dataexsys::adapters
