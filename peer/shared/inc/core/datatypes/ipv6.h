#ifndef IPV6_H
#define IPV6_H

#if !defined(__linux__)
#error "This project is optimized for Linux systems only"
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "shared/util/Size_Offsets.h"
#include "shared/datatype/OpStatus.h"

#define IPV6_INLINE inline __attribute__((always_inline))



/*
    Pure IPv6 = 128 bits exactly
    32-bit aligned for SIMD/cache friendliness
*/
typedef struct __attribute__((aligned(32))) {
    uint8_t address[16];
} ipv6_t;

/* -------------------------
   Init
--------------------------*/

IPV6_INLINE void ipv6_init(ipv6_t *ip, const uint8_t addr[16])
{
    memcpy(ip->address, addr, IPV6_SIZE);
}

IPV6_INLINE void ipv6_zero(ipv6_t *ip)
{
    memset(ip->address, 0, IPV6_SIZE);
}

/* -------------------------
   Serialization (NETWORK FORMAT)
   - 16 bytes raw copy
--------------------------*/

/* Moved to NetworkSerialization.h */


/* -------------------------
   Deserialization
   - reconstruct from network bytes
--------------------------*/

/* Moved to NetworkSerialization.h */


/* -------------------------
   Classification helpers
--------------------------*/

IPV6_INLINE bool ipv6_is_loopback(const ipv6_t *ip)
{
    return ip->address[15] == 1 &&
           memcmp(ip->address, (uint8_t[15]){0}, 15) == 0;
}

IPV6_INLINE bool ipv6_is_multicast(const ipv6_t *ip)
{
    return ip->address[0] == 0xFF;
}

IPV6_INLINE bool ipv6_is_link_local(const ipv6_t *ip)
{
    return ip->address[0] == 0xFE &&
           (ip->address[1] & 0xC0) == 0x80;
}

IPV6_INLINE bool ipv6_is_global(const ipv6_t *ip)
{
    return !ipv6_is_multicast(ip) &&
           !ipv6_is_link_local(ip);
}

/* -------------------------
   Accessor
--------------------------*/

IPV6_INLINE const uint8_t *ipv6_get_address(const ipv6_t *ip)
{
    return ip->address;
}

/* -------------------------
   String conversion
--------------------------*/

IPV6_INLINE void ipv6_to_string(const ipv6_t *ip, char *out, size_t len)
{
    if (len < 40) return;

    snprintf(out, len,
        "%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
        "%02x%02x:%02x%02x:%02x%02x:%02x%02x",
        ip->address[0], ip->address[1],
        ip->address[2], ip->address[3],
        ip->address[4], ip->address[5],
        ip->address[6], ip->address[7],
        ip->address[8], ip->address[9],
        ip->address[10], ip->address[11],
        ip->address[12], ip->address[13],
        ip->address[14], ip->address[15]
    );
}

#endif // IPV6_H