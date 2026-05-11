#pragma once

// =============================================================================
// Adapter.hpp
//
// IAdapter — universal adapter interface.
//
// INVARIANTS:
//  1. An adapter is either stopped or running.  push()/poll() MUST return
//     ADAPTER_NOT_STARTED if called before start().
//  2. start() / stop() are idempotent with respect to the stopped/running state.
//  3. push() accepts ownership of the message via move semantics.
//     The caller MUST NOT access the message after a successful push().
//  4. poll() is non-blocking.  It returns std::nullopt when the queue is empty.
//  5. Adapters MUST NOT spawn threads.  Async work goes through TaskSystem.
//  6. Adapters MUST NOT contain business logic, crypto, or consensus.
// =============================================================================

#if !defined(__linux__)
#error "Adapter layer: Linux only."
#endif

#include "AdapterMessage.hpp"
#include "AdapterResult.hpp"

#include <optional>
#include <string>

namespace dataexsys::adapters {

class IAdapter {
public:
    virtual ~IAdapter() = default;

    // -----------------------------------------------------------------------
    // Lifecycle
    // -----------------------------------------------------------------------

    /// Initialise internal state and mark the adapter as running.
    /// Returns false (and leaves adapter stopped) on initialisation failure.
    virtual bool start() = 0;

    /// Drain internal state and mark the adapter as stopped.
    /// Must be safe to call on a stopped adapter (no-op).
    virtual void stop() = 0;

    // -----------------------------------------------------------------------
    // Identity
    // -----------------------------------------------------------------------

    /// Human-readable adapter name used in diagnostics and logging.
    [[nodiscard]] virtual std::string name() const = 0;

    // -----------------------------------------------------------------------
    // Message passing
    // -----------------------------------------------------------------------

    /// Submit a message into this adapter's inbound queue.
    /// Takes ownership of the message via move — caller must not touch it after.
    /// Returns AdapterResult::fail(ADAPTER_NOT_STARTED) if not running.
    virtual AdapterResult push(AdapterMessage&& msg) = 0;

    /// Retrieve the next outbound message from this adapter's output queue.
    /// Non-blocking: returns std::nullopt when the queue is empty.
    virtual std::optional<AdapterMessage> poll() = 0;

    // -----------------------------------------------------------------------
    // Non-copyable, non-movable (adapters are registered by pointer)
    // -----------------------------------------------------------------------
    IAdapter(const IAdapter&)            = delete;
    IAdapter& operator=(const IAdapter&) = delete;
    IAdapter(IAdapter&&)                 = delete;
    IAdapter& operator=(IAdapter&&)      = delete;

protected:
    IAdapter() = default;
};

} // namespace dataexsys::adapters
