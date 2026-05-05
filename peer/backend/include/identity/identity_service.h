#ifndef IDENTITY_SERVICE_H
#define IDENTITY_SERVICE_H

#include "identity/identity.h"
#include "runtime/task/coro.h"
#include "runtime/sync/event.h"
#include <cstdint>
#include <mutex>

namespace identity {

/**
 * @brief Service responsible for managing node identity and cryptographic keys.
 * 
 * Uses the centralized async runtime for non-blocking identity generation.
 */
class IdentityService {
public:
    explicit IdentityService() = default;

    /**
     * @brief Asynchronously initialize the identity.
     * 
     * Spawns a background task to generate keys and gather system info.
     */
    void initialize();

    /**
     * @brief Returns a coroutine task that performs the initialization.
     */
    runtime::ResumableTask<> initialize_async();

    /**
     * @brief Returns an awaitable that suspends until the identity is ready.
     */
    auto wait_until_ready() { return ready_event; }

    // retrieve generated identity
    NodeIdentity getIdentity() const;

    // state check
    bool isReady() const;

private:
    mutable std::mutex mtx;
    NodeIdentity node_identity;
    bool ready = false;
    
    runtime::AsyncEvent ready_event;

    static NodeIdentity issueNodeIdentity();
};

} // namespace identity

#endif