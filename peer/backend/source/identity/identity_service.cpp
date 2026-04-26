#include "IdentityService.h"

#include <sodium.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <ctime>

// =====================================================
// Get FIRST IPv6 address (non-loopback only)
// =====================================================
static std::string getFirstGlobalIPv6() {
    struct ifaddrs *ifaddr = nullptr;

    if (getifaddrs(&ifaddr) == -1) {
        return "";
    }

    char ip[INET6_ADDRSTRLEN];

    for (auto ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {

        if (!ifa->ifa_addr) continue;

        if (ifa->ifa_addr->sa_family == AF_INET6) {

            auto *in6 = (sockaddr_in6*)ifa->ifa_addr;

            inet_ntop(AF_INET6, &in6->sin6_addr, ip, sizeof(ip));

            std::string address(ip);

            // Convert to binary form for prefix checking
            struct in6_addr raw = in6->sin6_addr;

            uint8_t firstByte = raw.s6_addr[0];

            // GLOBAL IPv6 range: 2000::/3 → first 3 bits = 001
            bool isGlobal =
                (firstByte & 0xE0) == 0x20;  // 0x20 = 001xxxxx

            if (isGlobal) {
                freeifaddrs(ifaddr);
                return address;   // ✅ FIRST GLOBAL IPv6 ONLY
            }
        }
    }

    freeifaddrs(ifaddr);
    return "";
}
// =====================================================
// MAIN IDENTITY GENERATION FUNCTION
// =====================================================
NodeIdentity IdentityService::issueNodeIdentity() {

    // init libsodium once
    if (sodium_init() < 0) {
        throw std::runtime_error("libsodium initialization failed");
    }

    NodeIdentity identity;

    // -------------------------
    // 1. IPv6
    // -------------------------
    identity.ipv6 = getFirstIPv6();

    // -------------------------
    // 2. X25519 Keypair
    // (Key exchange / encryption)
    // -------------------------
    crypto_kx_keypair(
        identity.x25519_public.data(),
        identity.x25519_private.data()
    );

    // -------------------------
    // 3. Ed25519 Keypair
    // (Signing / identity auth)
    // -------------------------
    crypto_sign_keypair(
        identity.ed25519_public.data(),
        identity.ed25519_private.data()
    );

    // -------------------------
    // 4. Metadata
    // -------------------------
    identity.created_at_unix = static_cast<uint64_t>(std::time(nullptr));
    identity.is_valid = true;

    return identity;
}