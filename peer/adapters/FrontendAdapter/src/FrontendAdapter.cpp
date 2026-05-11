// =============================================================================
// FrontendAdapter.cpp
// =============================================================================

#include "FrontendAdapter.hpp"

#include <algorithm>
#include <cstring>

namespace dataexsys::adapters {

FrontendAdapter::FrontendAdapter(AdapterId self_id) noexcept
    : self_id_(self_id) {}

FrontendAdapter::~FrontendAdapter() { stop(); }

bool FrontendAdapter::start()
{
    std::lock_guard lock(mutex_);
    if (running_) return true;
    running_ = true;
    return true;
}

void FrontendAdapter::stop()
{
    std::lock_guard lock(mutex_);
    running_ = false;
    outbound_.clear();
}

std::string FrontendAdapter::name() const { return "FrontendAdapter"; }

AdapterResult FrontendAdapter::push(AdapterMessage&& msg)
{
    if (!running_)
        return AdapterResult::fail(AdapterError::ADAPTER_NOT_STARTED);

    switch (msg.opcode) {
        case AdapterOpcode::FRONTEND_STATUS_UPDATE: {
            // Route incoming status update to outbound — frontend polls it.
            std::lock_guard lock(mutex_);
            outbound_.push_back(std::move(msg));
            return AdapterResult::success();
        }
        default:
            return AdapterResult::fail(AdapterError::UNSUPPORTED_OPCODE);
    }
}

std::optional<AdapterMessage> FrontendAdapter::poll()
{
    std::lock_guard lock(mutex_);
    if (outbound_.empty()) return std::nullopt;
    auto msg = std::move(outbound_.front());
    outbound_.pop_front();
    return msg;
}

AdapterResult FrontendAdapter::broadcast_status(std::string_view json_payload,
                                                  AdapterId        destination_id)
{
    if (!running_)
        return AdapterResult::fail(AdapterError::ADAPTER_NOT_STARTED);
    if (json_payload.empty())
        return AdapterResult::fail(AdapterError::PAYLOAD_EMPTY);

    // Structural JSON validation before encoding
    const BufferView json_view {
        reinterpret_cast<const uint8_t*>(json_payload.data()),
        json_payload.size()
    };

    if (!validate_json(json_view))
        return AdapterResult::fail(AdapterError::SERIALISATION_FAILED,
            "broadcast_status: malformed JSON payload");

    std::lock_guard lock(mutex_);
    outbound_.push_back(build_status_message(json_payload, destination_id));
    return AdapterResult::success();
}

AdapterResult FrontendAdapter::accept_frontend_request(BufferView json_bytes,
                                                         AdapterId  source_id)
{
    if (!running_)
        return AdapterResult::fail(AdapterError::ADAPTER_NOT_STARTED);
    if (json_bytes.empty())
        return AdapterResult::fail(AdapterError::PAYLOAD_EMPTY);
    if (!validate_json(json_bytes))
        return AdapterResult::fail(AdapterError::DESERIALISATION_FAILED,
            "accept_frontend_request: malformed JSON");

    // TODO: Parse `opcode` field from JSON, determine destination adapter,
    //       construct appropriate AdapterMessage and push to outbound.
    (void)source_id;
    return AdapterResult::fail(AdapterError::TRANSLATION_FAILED,
        "accept_frontend_request — JSON → opcode routing not yet implemented");
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

bool FrontendAdapter::validate_json(BufferView json_bytes) const noexcept
{
    // Minimal structural check: must start with '{' and end with '}'
    // ignoring leading/trailing ASCII whitespace.
    if (json_bytes.empty()) return false;
    std::size_t lo = 0, hi = json_bytes.size() - 1;
    while (lo <= hi && json_bytes[lo]  <= 0x20) ++lo;
    while (hi > lo  && json_bytes[hi]  <= 0x20) --hi;
    return json_bytes[lo] == '{' && json_bytes[hi] == '}';
}

AdapterMessage FrontendAdapter::build_status_message(std::string_view json,
                                                       AdapterId        destination_id)
{
    OwnedBuffer payload(
        reinterpret_cast<const uint8_t*>(json.data()),
        reinterpret_cast<const uint8_t*>(json.data()) + json.size());

    return AdapterMessage::make(
        AdapterOpcode::FRONTEND_STATUS_UPDATE,
        self_id_,
        destination_id,
        std::move(payload));
}

} // namespace dataexsys::adapters
