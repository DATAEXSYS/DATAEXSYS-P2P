#include "runtime/runtime.h"
#include "identity/identity_service.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <chrono>

using namespace runtime;
using namespace identity;

/**
 * @brief Demonstrates a coroutine that waits for a service to be ready.
 */
ResumableTask<> node_startup_sequence(IdentityService& identity_service) {
    // Ensure we are on a worker thread
    co_await Runtime::schedule();
    
    std::cout << "[Node] Starting startup sequence..." << std::endl;
    
    // Initialize identity service
    identity_service.initialize();
    
    // Wait for identity generation (Event-Driven)
    std::cout << "[Node] Waiting for identity generation event..." << std::endl;
    co_await identity_service.wait_until_ready();
    
    auto id = identity_service.getIdentity();
    std::cout << "\n================ NODE IDENTITY READY ================" << std::endl;
    std::cout << "IPv6: " << (id.ipv6.empty() ? "None (using local)" : id.ipv6) << std::endl;
    
    std::cout << "Public Key (Ed25519): ";
    for(int i=0; i<8; ++i) std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)id.ed25519_public[i];
    std::cout << "..." << std::dec << std::endl;
    
    std::cout << "Created At (Unix): " << id.created_at_unix << std::endl;
    std::cout << "====================================================\n" << std::endl;
    
    co_return;
}

int main() {
    std::cout << "DATAEXSYS P2P Peer Backend Starting..." << std::endl;
    
    auto& rt = Runtime::instance();
    IdentityService identity_service;

    // Spawn the startup sequence coroutine
    rt.spawn(node_startup_sequence(identity_service));

    // Also spawn some background noise to show concurrency
    for (int i = 0; i < 3; ++i) {
        rt.spawn([i]() {
            std::cout << "[Background] Task " << i << " running on thread " 
                      << std::this_thread::get_id() << std::endl;
        });
    }

    // Keep main alive to observe the async operations
    // In a production app, this would be a signal handler or a blocking UI/Network loop
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::cout << "[Main] Shutdown complete." << std::endl;
    return 0;
}