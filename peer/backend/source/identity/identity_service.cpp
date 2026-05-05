#include "identity/identity_service.h"
#include "runtime/runtime.h"
#include <sodium.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <cstdint>
#include <ctime>
#include <stdexcept>
#include <iostream>

namespace identity {

static std::string getFirstGlobalIPv6() {
    struct ifaddrs* ifaddr = nullptr;

    if (getifaddrs(&ifaddr) == -1) {
        return "";
    }

    char ip[INET6_ADDRSTRLEN];

    for (auto ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET6) {
            auto* in6 = (sockaddr_in6*)ifa->ifa_addr;

            inet_ntop(
                AF_INET6,
                &in6->sin6_addr,
                ip,
                sizeof(ip)
            );

            std::string address(ip);
            struct in6_addr raw = in6->sin6_addr;
            uint8_t firstByte = raw.s6_addr[0];

            // Global range = 2000::/3
            bool isGlobal = (firstByte & 0xE0) == 0x20;

            if (isGlobal) {
                freeifaddrs(ifaddr);
                return address;
            }
        }
    }

    freeifaddrs(ifaddr);
    return "";
}

void IdentityService::initialize() {
    // Services should just spawn tasks and return
    runtime::Runtime::instance().spawn(initialize_async());
}

runtime::ResumableTask<> IdentityService::initialize_async() {
    // Switch to a worker thread immediately
    co_await runtime::Runtime::schedule();

    std::cout << "[IdentityService] Starting async identity generation..." << std::endl;

    NodeIdentity generated = issueNodeIdentity();

    {
        std::lock_guard<std::mutex> lock(mtx);
        node_identity = generated;
        ready = true;
    }

    std::cout << "[IdentityService] Identity initialized successfully." << std::endl;
    ready_event.set();
    co_return;
}

NodeIdentity IdentityService::getIdentity() const {
    std::lock_guard<std::mutex> lock(mtx);
    return node_identity;
}

bool IdentityService::isReady() const {
    std::lock_guard<std::mutex> lock(mtx);
    return ready;
}

NodeIdentity IdentityService::issueNodeIdentity() {
    if (sodium_init() < 0) {
        throw std::runtime_error("libsodium initialization failed");
    }

    NodeIdentity identity;

    // IPv6
    identity.ipv6 = getFirstGlobalIPv6();

    // X25519
    crypto_kx_keypair(
        identity.x25519_public.data(),
        identity.x25519_private.data()
    );

    // Ed25519
    crypto_sign_keypair(
        identity.ed25519_public.data(),
        identity.ed25519_private.data()
    );

    // Metadata
    identity.created_at_unix = static_cast<uint64_t>(std::time(nullptr));
    identity.is_valid = true;

    return identity;
}

} // namespace identity