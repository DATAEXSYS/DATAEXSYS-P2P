// =============================================================================
// NetworkAdapter.cpp
// =============================================================================

#include "NetworkAdapter.hpp"

namespace dataexsys::adapters {

NetworkAdapter::NetworkAdapter(AdapterId self_id) noexcept
    : self_id_(self_id) {}

NetworkAdapter::~NetworkAdapter() { stop(); }

bool NetworkAdapter::start()
{
    std::lock_guard lock(mutex_);
    if (running_) return true;
    running_ = true;
    return true;
}

void NetworkAdapter::stop()
{
    std::lock_guard lock(mutex_);
    running_ = false;
    outbound_.clear();
}

std::string NetworkAdapter::name() const { return "NetworkAdapter"; }

AdapterResult NetworkAdapter::push(AdapterMessage&& msg)
{
    std::lock_guard lock(mutex_);
    if (!running_)
        return AdapterResult::fail(AdapterError::ADAPTER_NOT_STARTED);

    switch (msg.opcode) {
        case AdapterOpcode::NETWORK_PACKET_SEND:
            // TODO: Marshal msg.payload to wire format and hand to socket layer.
            // For now, echo it back as a loopback (stub behaviour).
            outbound_.push_back(std::move(msg));
            return AdapterResult::success();

        default:
            return AdapterResult::fail(AdapterError::UNSUPPORTED_OPCODE,
                "NetworkAdapter only handles NETWORK_PACKET_SEND via push()");
    }
}

std::optional<AdapterMessage> NetworkAdapter::poll()
{
    std::lock_guard lock(mutex_);
    if (outbound_.empty()) return std::nullopt;
    auto msg = std::move(outbound_.front());
    outbound_.pop_front();
    return msg;
}

AdapterResult NetworkAdapter::ingest_raw(BufferView raw_bytes, AdapterId destination_id)
{
    std::lock_guard lock(mutex_);
    if (!running_)
        return AdapterResult::fail(AdapterError::ADAPTER_NOT_STARTED);
    if (raw_bytes.empty())
        return AdapterResult::fail(AdapterError::PAYLOAD_EMPTY);

    // Wrap raw bytes as NETWORK_PACKET_RECEIVED — no interpretation here.
    auto msg = AdapterMessage::from_view(
        AdapterOpcode::NETWORK_PACKET_RECEIVED,
        self_id_,
        destination_id,
        raw_bytes);

    outbound_.push_back(std::move(msg));
    return AdapterResult::success();
}

OwnedBuffer NetworkAdapter::marshal_for_wire(const AdapterMessage& msg) const
{
    // TODO: Implement packet framing (length prefix, header, payload).
    // Stub: return a copy of the raw payload as-is.
    OwnedBuffer wire(msg.payload.begin(), msg.payload.end());
    return wire;
}

} // namespace dataexsys::adapters
