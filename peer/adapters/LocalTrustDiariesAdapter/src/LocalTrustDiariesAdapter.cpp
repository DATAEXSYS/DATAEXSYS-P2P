// =============================================================================
// LocalTrustDiariesAdapter.cpp
//
// TrustNode from LocalTrustDiaries is used here and ONLY here.
// The pimpl pattern hides it completely from the .hpp header.
// =============================================================================

#include "LocalTrustDiariesAdapter.hpp"

// LocalTrustDiaries is header-only — include directly.
// TrustNode stays confined to this translation unit.
#include "TrustNode/TrustNode.h"
#include "Types/AckType.h"

#include <algorithm>
#include <cstring>
#include <vector>

namespace dataexsys::adapters {

// ---------------------------------------------------------------------------
// TrustSnapshot serialisation
// ---------------------------------------------------------------------------

OwnedBuffer TrustSnapshot::serialise() const
{
    OwnedBuffer buf(13);
    buf[0] = node_id;
    // Little-endian encoding for the 32-bit fields
    std::memcpy(buf.data() + 1, &trust, 4);
    std::memcpy(buf.data() + 5, &acks,  4);
    std::memcpy(buf.data() + 9, &nacks, 4);
    return buf;
}

std::optional<TrustSnapshot> TrustSnapshot::deserialise(BufferView view)
{
    if (view.size() < 13) return std::nullopt;
    TrustSnapshot snap{};
    snap.node_id = view[0];
    std::memcpy(&snap.trust, view.data() + 1, 4);
    std::memcpy(&snap.acks,  view.data() + 5, 4);
    std::memcpy(&snap.nacks, view.data() + 9, 4);
    return snap;
}

// ---------------------------------------------------------------------------
// Pimpl — hides TrustNode from header
// ---------------------------------------------------------------------------

struct LocalTrustDiariesAdapter::Impl {
    std::vector<TrustNode*> nodes;
    std::size_t             max_peers;

    explicit Impl(std::size_t max) : max_peers(max) {}
    ~Impl() {
        for (auto* n : nodes) delete n;
    }

    TrustNode* find(uint8_t node_id) const {
        for (auto* n : nodes)
            if (n->nodeid == node_id) return n;
        return nullptr;
    }

    TrustNode* find_or_create(uint8_t node_id) {
        if (auto* n = find(node_id)) return n;
        if (nodes.size() >= max_peers) return nullptr;
        auto* n = new TrustNode{};
        trust_node_init(n);
        trust_node_set_id(n, node_id);
        nodes.push_back(n);
        return n;
    }
};

// ---------------------------------------------------------------------------
// LocalTrustDiariesAdapter
// ---------------------------------------------------------------------------

LocalTrustDiariesAdapter::LocalTrustDiariesAdapter(AdapterId   self_id,
                                                     std::size_t max_peers) noexcept
    : self_id_(self_id), max_peers_(max_peers) {}

LocalTrustDiariesAdapter::~LocalTrustDiariesAdapter()
{
    stop();
    delete impl_;
}

bool LocalTrustDiariesAdapter::start()
{
    std::lock_guard lock(mutex_);
    if (running_) return true;
    impl_    = new Impl(max_peers_);
    running_ = true;
    return true;
}

void LocalTrustDiariesAdapter::stop()
{
    std::lock_guard lock(mutex_);
    running_ = false;
    outbound_.clear();
    delete impl_;
    impl_ = nullptr;
}

std::string LocalTrustDiariesAdapter::name() const
{
    return "LocalTrustDiariesAdapter";
}

AdapterResult LocalTrustDiariesAdapter::push(AdapterMessage&& msg)
{
    if (!running_)
        return AdapterResult::fail(AdapterError::ADAPTER_NOT_STARTED);

    switch (msg.opcode) {
        case AdapterOpcode::TRUST_ACK:  return handle_ack(std::move(msg));
        case AdapterOpcode::TRUST_NACK: return handle_nack(std::move(msg));
        default:
            return AdapterResult::fail(AdapterError::UNSUPPORTED_OPCODE);
    }
}

std::optional<AdapterMessage> LocalTrustDiariesAdapter::poll()
{
    std::lock_guard lock(mutex_);
    if (outbound_.empty()) return std::nullopt;
    auto msg = std::move(outbound_.front());
    outbound_.pop_front();
    return msg;
}

std::optional<TrustSnapshot> LocalTrustDiariesAdapter::lookup(uint8_t node_id) const
{
    std::lock_guard lock(mutex_);
    if (!impl_) return std::nullopt;
    const auto* n = impl_->find(node_id);
    if (!n) return std::nullopt;
    return TrustSnapshot {
        n->nodeid,
        n->trust,
        n->acks,
        n->nacks,
    };
}

AdapterResult LocalTrustDiariesAdapter::record_ack(uint8_t node_id)
{
    std::lock_guard lock(mutex_);
    if (!running_) return AdapterResult::fail(AdapterError::ADAPTER_NOT_STARTED);
    auto* n = impl_->find_or_create(node_id);
    if (!n) return AdapterResult::fail(AdapterError::QUEUE_FULL, "peer table full");
    trust_node_set_acks(n, n->acks + 1);
    // trust_node_update_trust() body currently empty in library — called for future compat
    trust_node_update_trust(n, 1 /* ACK_TYPE */);
    return AdapterResult::success();
}

AdapterResult LocalTrustDiariesAdapter::record_nack(uint8_t node_id)
{
    std::lock_guard lock(mutex_);
    if (!running_) return AdapterResult::fail(AdapterError::ADAPTER_NOT_STARTED);
    auto* n = impl_->find_or_create(node_id);
    if (!n) return AdapterResult::fail(AdapterError::QUEUE_FULL, "peer table full");
    trust_node_set_nacks(n, n->nacks + 1);
    trust_node_update_trust(n, 0 /* NACK_TYPE */);
    return AdapterResult::success();
}

AdapterResult LocalTrustDiariesAdapter::broadcast_trust_snapshot(AdapterId destination_id)
{
    std::lock_guard lock(mutex_);
    if (!running_) return AdapterResult::fail(AdapterError::ADAPTER_NOT_STARTED);

    // Concatenate all snapshots: [count(2 LE)][snapshot x N]
    OwnedBuffer payload;
    const uint16_t count = static_cast<uint16_t>(impl_->nodes.size());
    payload.resize(2 + count * 13);
    std::memcpy(payload.data(), &count, 2);
    for (std::size_t i = 0; i < impl_->nodes.size(); ++i) {
        const auto* n = impl_->nodes[i];
        TrustSnapshot snap { n->nodeid, n->trust, n->acks, n->nacks };
        auto serialised = snap.serialise();
        std::memcpy(payload.data() + 2 + i * 13, serialised.data(), 13);
    }

    outbound_.push_back(
        AdapterMessage::make(AdapterOpcode::FRONTEND_STATUS_UPDATE,
                             self_id_, destination_id, std::move(payload)));
    return AdapterResult::success();
}

// ---------------------------------------------------------------------------
// Private handlers
// ---------------------------------------------------------------------------

AdapterResult LocalTrustDiariesAdapter::handle_ack(AdapterMessage&& msg)
{
    // Payload: [node_id (1 byte)]
    if (msg.payload.size() < 1)
        return AdapterResult::fail(AdapterError::PAYLOAD_EMPTY);
    return record_ack(msg.payload[0]);
}

AdapterResult LocalTrustDiariesAdapter::handle_nack(AdapterMessage&& msg)
{
    if (msg.payload.size() < 1)
        return AdapterResult::fail(AdapterError::PAYLOAD_EMPTY);
    return record_nack(msg.payload[0]);
}

} // namespace dataexsys::adapters
