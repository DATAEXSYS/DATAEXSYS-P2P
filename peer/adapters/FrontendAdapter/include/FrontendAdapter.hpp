#pragma once

// =============================================================================
// FrontendAdapter.hpp
//
// IPC bridge between nodeEngine and the Qt/QML frontend.
//
// RESPONSIBILITIES:
//  - Serialise nodeEngine state snapshots to JSON-compatible byte payloads.
//  - Accept frontend requests (serialised JSON) and normalise to AdapterMessage.
//  - Broadcast FRONTEND_STATUS_UPDATE events to consumers.
//
// INVARIANTS:
//  - Qt/QML MUST NOT directly touch nodeEngine internals.
//  - This adapter MUST NOT link Qt — it produces plain C++23 byte payloads.
//    The frontend layer links this adapter and Qt separately.
//  - All JSON translation is done via hand-written serialisation helpers
//    or a zero-dependency library — NOT via Qt's JSON classes.
//    (Qt JSON may be used in the frontend itself, not here.)
// =============================================================================

#if !defined(__linux__)
#error "FrontendAdapter: Linux only."
#endif

#include "Adapter.hpp"

#include <deque>
#include <mutex>
#include <string>
#include <string_view>

namespace dataexsys::adapters {

class FrontendAdapter final : public IAdapter {
public:
    explicit FrontendAdapter(AdapterId self_id) noexcept;
    ~FrontendAdapter() override;

    // -----------------------------------------------------------------------
    // IAdapter
    // -----------------------------------------------------------------------
    bool                          start()  override;
    void                          stop()   override;
    [[nodiscard]] std::string     name()   const override;
    AdapterResult                 push(AdapterMessage&& msg) override;
    std::optional<AdapterMessage> poll()   override;

    // -----------------------------------------------------------------------
    // FrontendAdapter-specific API
    // -----------------------------------------------------------------------

    /// Broadcast a status update to the frontend.
    /// `json_payload` is a UTF-8 JSON string (validated before transmission).
    /// The string is encoded as UTF-8 bytes in the AdapterMessage payload.
    AdapterResult broadcast_status(std::string_view json_payload,
                                   AdapterId        destination_id);

    /// Accept a frontend request encoded as a JSON byte payload.
    /// Parses the opcode field from JSON and routes to the appropriate
    /// adapter via the outbound queue.
    AdapterResult accept_frontend_request(BufferView json_bytes,
                                          AdapterId  source_id);

private:
    AdapterId                  self_id_;
    bool                       running_ = false;

    std::deque<AdapterMessage> outbound_;
    mutable std::mutex         mutex_;

    // -----------------------------------------------------------------------
    // Internal helpers
    // -----------------------------------------------------------------------

    /// Validate that `json_bytes` is a well-formed UTF-8 JSON object.
    /// Does NOT parse semantics — only structural check.
    bool validate_json(BufferView json_bytes) const noexcept;

    /// Build a FRONTEND_STATUS_UPDATE message from a raw JSON string.
    AdapterMessage build_status_message(std::string_view json,
                                        AdapterId        destination_id);
};

} // namespace dataexsys::adapters
