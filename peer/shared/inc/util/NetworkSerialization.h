#ifndef UTIL_NETWORK_SERIALIZATION_H
#define UTIL_NETWORK_SERIALIZATION_H

#include "pkcertchain_config.h"

#include <endian.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>

/*
 * SERIALIZATION: Host byte order to Network byte order (Big Endian)
 */
static inline void serialize_u8(uint8_t v, uint8_t *out)
{
    out[0] = v;
}

static inline void serialize_u16_be(uint16_t v, uint8_t *out)
{
    uint16_t be = htobe16(v);
    memcpy(out, &be, 2);
}

static inline void serialize_u32_be(uint32_t v, uint8_t *out)
{
    uint32_t be = htobe32(v);
    memcpy(out, &be, 4);
}

static inline void serialize_u64_be(uint64_t v, uint8_t *out)
{
    uint64_t be = htobe64(v);
    memcpy(out, &be, 8);
}

static inline void serialize_def(const void *value, size_t size, uint8_t *out)
{
    memcpy(out, value, size);
}

/*
 * DESERIALIZATION: Network byte order (Big Endian) to Host byte order
 */
static inline void deserialize_u8_def(const uint8_t *in, uint8_t *out, size_t size)
{
    (void)size;
    memcpy(out, in, sizeof(uint8_t));
}

static inline void deserialize_u16_be(const uint8_t *in, uint16_t *out, size_t size)
{
    (void)size;
    uint16_t v;
    memcpy(&v, in, sizeof(uint16_t));
    *out = be16toh(v);
}

static inline void deserialize_u32_be(const uint8_t *in, uint32_t *out, size_t size)
{
    (void)size;
    uint32_t v;
    memcpy(&v, in, sizeof(uint32_t));
    *out = be32toh(v);
}

static inline void deserialize_u64_be(const uint8_t *in, uint64_t *out, size_t size)
{
    (void)size;
    uint64_t v;
    memcpy(&v, in, sizeof(uint64_t));
    *out = be64toh(v);
}

/*
 * DESERIALIZATION: Little Endian to Host byte order (for legacy/def compatibility)
 */
static inline void deserialize_u16_def(const uint8_t *in, uint16_t *out, size_t size)
{
    (void)size;
    uint16_t v;
    memcpy(&v, in, sizeof(uint16_t));
    *out = le16toh(v);
}

static inline void deserialize_u32_def(const uint8_t *in, uint32_t *out, size_t size)
{
    (void)size;
    uint32_t v;
    memcpy(&v, in, sizeof(uint32_t));
    *out = le32toh(v);
}

static inline void deserialize_u64_def(const uint8_t *in, uint64_t *out, size_t size)
{
    (void)size;
    uint64_t v;
    memcpy(&v, in, sizeof(uint64_t));
    *out = le64toh(v);
}

static inline void deserialize_def(const uint8_t *in, void *value, size_t size)
{
    memcpy(value, in, size);
}

#endif // UTIL_NETWORK_SERIALIZATION_H
