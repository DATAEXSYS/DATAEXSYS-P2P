#ifndef UTIL_NETWORK_SERIALIZATION_H
#define UTIL_NETWORK_SERIALIZATION_H



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

/* --- CUSTOM STRUCT SERIALIZATION --- */


#include "core/enums/OpStatus.h"
#include "core/enums/Tier.h"
#include "core/enums/AckType.h"
#include "core/Global_Size_Offsets.h"

// Forward or explicit includes for structures
#include "core/datatypes/ipv6.h"
#include "protocol/blockchain/certificate.h"
#include "protocol/blockchain/block.h"
#include "protocol/proofs/mini_pow/mini_pow_result.h"
#include "protocol/proofs/tier_pow/tier_pow_challenge_t.h"
#include "protocol/proofs/tier_pow/tier_pow_solve_t.h"
#include "protocol/proofs/tier_pow/TierPowResult.h"
#include "protocol/trust/TrustNode.h"
#include "protocol/routing/RoutingPacket.h"

// Macros needed by extracted functions
#ifndef CERT_INLINE
#define CERT_INLINE static inline
#endif
#ifndef BLOCK_INLINE
#define BLOCK_INLINE static inline
#endif
#ifndef IPV6_INLINE
#define IPV6_INLINE static inline
#endif
#ifndef TIER_POW_CHALLENGE_INLINE
#define TIER_POW_CHALLENGE_INLINE static inline
#endif
#ifndef TIER_POW_SOLVE_INLINE
#define TIER_POW_SOLVE_INLINE static inline
#endif
#ifndef TRUST_NODE_INLINE
#define TRUST_NODE_INLINE static inline
#endif

static inline OpStatus_t uint256_serialize_be(const uint256 *u, uint8_t *out, size_t out_size);
static inline OpStatus_t uint256_deserialize_be(const uint8_t *in, size_t in_size, uint256 *u);
static inline OpStatus_t uint512_serialize_be(const uint512 *u, uint8_t *out, size_t out_size);
static inline OpStatus_t uint512_deserialize_be(const uint8_t *in, size_t in_size, uint512 *u);
static inline int ipv6_serialize(const ipv6_t *ip, uint8_t *out, size_t out_size);
static inline int ipv6_deserialize(const uint8_t *in, size_t in_size, ipv6_t *ip);

CERT_INLINE OpStatus_t cert_serialize(const certificate *cert,
                                      uint8_t *out,
                                      size_t out_size)
{
    if (!cert || !out)
        return OP_NULL_PTR;

    if (out_size < CERT_SIZE)
        return OP_BUF_TOO_SMALL;

    size_t offset = 0;

    /* pubSignKey */
    if (uint256_serialize_be(&cert->pubSignKey,
                             out + offset,
                             UINT256_SIZE) != OP_SUCCESS)
    {
        return OP_INVALID_INPUT;
    }

    offset += UINT256_SIZE;

    /* pubEncKey */
    if (uint256_serialize_be(&cert->pubEncKey,
                             out + offset,
                             UINT256_SIZE) != OP_SUCCESS)
    {
        return OP_INVALID_INPUT;
    }

    offset += UINT256_SIZE;

    /* IPv6 id */
    if (ipv6_serialize(&cert->id,
                       out + offset,
                       IPV6_SIZE) != 0)
    {
        return OP_INVALID_INPUT;
    }

    offset += IPV6_SIZE;

    /* reserved */
    memcpy(out + offset,
           cert->reserved,
           sizeof(cert->reserved));

    return OP_SUCCESS;
}

CERT_INLINE OpStatus_t cert_deserialize(const uint8_t *in,
                                        size_t in_size,
                                        certificate *cert)
{
    if (!cert || !in)
        return OP_NULL_PTR;

    if (in_size < CERT_SIZE)
        return OP_BUF_TOO_SMALL;

    size_t offset = 0;

    /* pubSignKey */
    if (uint256_deserialize_be(in + offset,
                               UINT256_SIZE,
                               &cert->pubSignKey) != OP_SUCCESS)
    {
        return OP_INVALID_INPUT;
    }

    offset += UINT256_SIZE;

    /* pubEncKey */
    if (uint256_deserialize_be(in + offset,
                               UINT256_SIZE,
                               &cert->pubEncKey) != OP_SUCCESS)
    {
        return OP_INVALID_INPUT;
    }

    offset += UINT256_SIZE;

    /* IPv6 id */
    if (ipv6_deserialize(in + offset,
                         IPV6_SIZE,
                         &cert->id) != 0)
    {
        return OP_INVALID_INPUT;
    }

    offset += IPV6_SIZE;

    /* reserved */
    memcpy(cert->reserved,
           in + offset,
           sizeof(cert->reserved));

    return OP_SUCCESS;
}

U512_INLINE OpStatus_t uint512_serialize_be(const uint512 *u, uint8_t *out, size_t out_size)
{
    if (!u || !out) return OP_NULL_PTR;
    if (out_size < UINT512_SIZE) return OP_BUF_TOO_SMALL;
    for (int i = 0; i < 8; ++i) {
        serialize_u64_be(u->w[i], out + (i * 8));
    }
    return OP_SUCCESS;
}

U512_INLINE OpStatus_t uint512_deserialize_be(const uint8_t *in, size_t in_size, uint512 *u)
{
    if (!u || !in) return OP_NULL_PTR;
    if (in_size < UINT512_SIZE) return OP_BUF_TOO_SMALL;
    for (int i = 0; i < 8; ++i) {
        deserialize_u64_be(in + (i * 8), &u->w[i], sizeof(uint64_t));
    }
    return OP_SUCCESS;
}

U256_INLINE OpStatus_t uint256_serialize_be(const uint256 *u, uint8_t *out, size_t out_size)
{
    if (!u || !out) return OP_NULL_PTR;
    if (out_size < UINT256_SIZE) return OP_BUF_TOO_SMALL;
    for (int i = 0; i < 4; ++i) {
        serialize_u64_be(u->w[i], out + (i * 8));
    }
    return OP_SUCCESS;
}

U256_INLINE OpStatus_t uint256_deserialize_be(const uint8_t *in, size_t in_size, uint256 *u)
{
    if (!u || !in) return OP_NULL_PTR;
    if (in_size < UINT256_SIZE) return OP_BUF_TOO_SMALL;
    for (int i = 0; i < 4; ++i) {
        deserialize_u64_be(in + (i * 8), &u->w[i], sizeof(uint64_t));
    }
    return OP_SUCCESS;
}

U256_INLINE OpStatus_t uint256_serialize_two_be(const uint256 *a, const uint256 *b, uint8_t *out, size_t out_size)
{
    if (!a || !b || !out) return OP_NULL_PTR;
    if (out_size < (UINT256_SIZE * 2)) return OP_BUF_TOO_SMALL;
    if (uint256_serialize_be(a, out, UINT256_SIZE) != OP_SUCCESS) return OP_INVALID_INPUT;
    if (uint256_serialize_be(b, out + UINT256_SIZE, UINT256_SIZE) != OP_SUCCESS) return OP_INVALID_INPUT;
    return OP_SUCCESS;
}

IPV6_INLINE int ipv6_serialize(const ipv6_t *ip,
                               uint8_t *out,
                               size_t out_size)
{
    if (!ip || !out) return -1;
    if (out_size < IPV6_SIZE) return -2;

    memcpy(out, ip->address, IPV6_SIZE);
    return 0;
}

IPV6_INLINE int ipv6_deserialize(const uint8_t *in,
                                 size_t in_size,
                                 ipv6_t *ip)
{
    if (!ip || !in) return -1;
    if (in_size < IPV6_SIZE) return -2;

    memcpy(ip->address, in, IPV6_SIZE);
    return 0;
}



static inline __attribute__((always_inline)) OpStatus_t minipowresult_serialize(const MiniPowResult *res, uint8_t *out, size_t out_size);
static inline __attribute__((always_inline)) OpStatus_t minipowresult_deserialize(const uint8_t *in, size_t in_size, MiniPowResult *res);
static inline OpStatus_t tierpowresult_serialize(const TierPowResult *res, uint8_t *out, size_t out_size);
static inline OpStatus_t tierpowresult_deserialize(const uint8_t *in, size_t in_size, TierPowResult *res);

BLOCK_INLINE OpStatus_t block_serialize(const block *blk, uint8_t *out, size_t out_size)
{
    if (!blk || !out) return OP_NULL_PTR;
    if (out_size < BLOCK_SERIALIZED_SIZE) return OP_BUF_TOO_SMALL;

    if (cert_serialize(&blk->cert, out, CERT_SIZE) != OP_SUCCESS) return OP_INVALID_INPUT;
    if (uint256_serialize_be(&blk->CurrentCertHash, out + CERT_SIZE, UINT256_SIZE) != OP_SUCCESS) return OP_INVALID_INPUT;
    if (uint256_serialize_be(&blk->prevHash, out + CERT_SIZE + UINT256_SIZE, UINT256_SIZE) != OP_SUCCESS) return OP_INVALID_INPUT;
    if (uint512_serialize_be(&blk->SignedByVerifier, out + CERT_SIZE + 2 * UINT256_SIZE, UINT512_SIZE) != OP_SUCCESS) return OP_INVALID_INPUT;
    serialize_u64_be(blk->height, out + CERT_SIZE + 2 * UINT256_SIZE + UINT512_SIZE);
    serialize_u64_be(blk->timestamp, out + CERT_SIZE + 2 * UINT256_SIZE + UINT512_SIZE + UINT64_SIZE);
    serialize_u8(blk->tier, out + CERT_SIZE + 2 * UINT256_SIZE + UINT512_SIZE + 2 * UINT64_SIZE);
    memcpy(out + CERT_SIZE + 2 * UINT256_SIZE + UINT512_SIZE + 2 * UINT64_SIZE + 1,
           blk->reserved, sizeof(blk->reserved));
    if (minipowresult_serialize(&blk->miniPowResult, out + BLOCK_SIZE, MINI_POW_RESULT_SERIALIZED_SIZE) != OP_SUCCESS) return OP_INVALID_INPUT;
    if (tierpowresult_serialize(&blk->tierPoWResult, out + BLOCK_SIZE + MINI_POW_RESULT_SERIALIZED_SIZE, TIER_POW_RESULT_SERIALIZED_SIZE) != OP_SUCCESS) return OP_INVALID_INPUT;
    return OP_SUCCESS;
}

BLOCK_INLINE OpStatus_t block_deserialize(const uint8_t *in, size_t in_size, block *blk)
{
    if (!blk || !in) return OP_NULL_PTR;
    if (in_size < BLOCK_SERIALIZED_SIZE) return OP_BUF_TOO_SMALL;

    if (cert_deserialize(in, CERT_SIZE, &blk->cert) != OP_SUCCESS) return OP_INVALID_INPUT;
    if (uint256_deserialize_be(in + CERT_SIZE, UINT256_SIZE, &blk->CurrentCertHash) != OP_SUCCESS) return OP_INVALID_INPUT;
    if (uint256_deserialize_be(in + CERT_SIZE + UINT256_SIZE, UINT256_SIZE, &blk->prevHash) != OP_SUCCESS) return OP_INVALID_INPUT;
    if (uint512_deserialize_be(in + CERT_SIZE + 2 * UINT256_SIZE, UINT512_SIZE, &blk->SignedByVerifier) != OP_SUCCESS) return OP_INVALID_INPUT;
    deserialize_u64_be(in + CERT_SIZE + 2 * UINT256_SIZE + UINT512_SIZE, &blk->height, sizeof(uint64_t));
    deserialize_u64_be(in + CERT_SIZE + 2 * UINT256_SIZE + UINT512_SIZE + UINT64_SIZE, &blk->timestamp, sizeof(uint64_t));
    deserialize_u8_def(in + CERT_SIZE + 2 * UINT256_SIZE + UINT512_SIZE + 2 * UINT64_SIZE, &blk->tier, sizeof(uint8_t));
    memcpy(blk->reserved,
           in + CERT_SIZE + 2 * UINT256_SIZE + UINT512_SIZE + 2 * UINT64_SIZE + 1,
           sizeof(blk->reserved));
    if (minipowresult_deserialize(in + BLOCK_SIZE, MINI_POW_RESULT_SERIALIZED_SIZE, &blk->miniPowResult) != OP_SUCCESS) return OP_INVALID_INPUT;
    if (tierpowresult_deserialize(in + BLOCK_SIZE + MINI_POW_RESULT_SERIALIZED_SIZE, TIER_POW_RESULT_SERIALIZED_SIZE, &blk->tierPoWResult) != OP_SUCCESS) return OP_INVALID_INPUT;
    return OP_SUCCESS;
}

TIER_POW_SOLVE_INLINE OpStatus_t tier_pow_solve_serialize(const tier_pow_solve_t *pow,
                                                          uint8_t *out,
                                                          size_t out_size)
{
    if (!pow || !out) return OP_NULL_PTR;
    if (out_size < TIER_POW_SOLVE_SERIALIZED_SIZE) return OP_BUF_TOO_SMALL;

    serialize_u64_be(pow->nonce, out);
    serialize_u8(pow->complexity, out + UINT64_SIZE);
    serialize_u64_be(pow->challenge_id, out + UINT64_SIZE + 1);
    memcpy(out + UINT64_SIZE + 1 + UINT64_SIZE, pow->reserved, sizeof(pow->reserved));
    return OP_SUCCESS;
}

TIER_POW_SOLVE_INLINE OpStatus_t tier_pow_solve_deserialize(const uint8_t *in,
                                                            size_t in_size,
                                                            tier_pow_solve_t *pow)
{
    if (!pow || !in) return OP_NULL_PTR;
    if (in_size < TIER_POW_SOLVE_SERIALIZED_SIZE) return OP_BUF_TOO_SMALL;

    deserialize_u64_be(in, &pow->nonce, sizeof(uint64_t));
    pow->complexity = in[UINT64_SIZE];
    deserialize_u64_be(in + UINT64_SIZE + 1, &pow->challenge_id, sizeof(uint64_t));
    memcpy(pow->reserved, in + UINT64_SIZE + 1 + UINT64_SIZE, sizeof(pow->reserved));
    return OP_SUCCESS;
}

TIER_POW_CHALLENGE_INLINE OpStatus_t tier_pow_challenge_serialize(const tier_pow_challenge_t *pow,
                                                                  uint8_t *out,
                                                                  size_t out_size)
{
    if (!pow || !out) return OP_NULL_PTR;
    if (out_size < TIER_POW_CHALLENGE_SERIALIZED_SIZE) return OP_BUF_TOO_SMALL;

    if (uint256_serialize_be(&pow->challenge, out, UINT256_SIZE) != OP_SUCCESS) return OP_INVALID_INPUT;
    serialize_u8(pow->complexity, out + UINT256_SIZE);
    serialize_u64_be(pow->challenge_id, out + UINT256_SIZE + 1);
    memcpy(out + UINT256_SIZE + 1 + UINT64_SIZE, pow->reserved, sizeof(pow->reserved));
    return OP_SUCCESS;
}

TIER_POW_CHALLENGE_INLINE OpStatus_t tier_pow_challenge_deserialize(const uint8_t *in,
                                                                    size_t in_size,
                                                                    tier_pow_challenge_t *pow)
{
    if (!pow || !in) return OP_NULL_PTR;
    if (in_size < TIER_POW_CHALLENGE_SERIALIZED_SIZE) return OP_BUF_TOO_SMALL;

    if (uint256_deserialize_be(in, UINT256_SIZE, &pow->challenge) != OP_SUCCESS) return OP_INVALID_INPUT;
    pow->complexity = in[UINT256_SIZE];
    deserialize_u64_be(in + UINT256_SIZE + 1, &pow->challenge_id, sizeof(uint64_t));
    memcpy(pow->reserved, in + UINT256_SIZE + 1 + UINT64_SIZE, sizeof(pow->reserved));
    return OP_SUCCESS;
}

static inline OpStatus_t tierpowresult_serialize(const TierPowResult *res, uint8_t *out, size_t out_size) {
    if (!res || !out) return OP_NULL_PTR;
    if (out_size < TIER_POW_RESULT_SERIALIZED_SIZE) return OP_BUF_TOO_SMALL;

    size_t off = 0;
    out[off++] = res->tier;
    out[off++] = res->reserved[0];
    out[off++] = res->reserved[1];
    out[off++] = res->reserved[2];

    uint256_serialize_be(&res->challenge.challenge, out + off, UINT256_SIZE);
    off += UINT256_SIZE;
    out[off++] = res->challenge.complexity;
    serialize_u64_be(res->challenge.challenge_id, out + off);
    off += 8;
    out[off++] = res->challenge.reserved[0];
    out[off++] = res->challenge.reserved[1];
    out[off++] = res->challenge.reserved[2];

    serialize_u64_be(res->solve.nonce, out + off);
    off += 8;
    out[off++] = res->solve.complexity;
    serialize_u64_be(res->solve.challenge_id, out + off);
    off += 8;
    out[off++] = res->solve.reserved[0];
    out[off++] = res->solve.reserved[1];
    out[off++] = res->solve.reserved[2];

    uint64_t time_bits;
    memcpy(&time_bits, &res->time_taken, 8);
    serialize_u64_be(time_bits, out + off);
    off += 8;

    return OP_SUCCESS;
}

static inline OpStatus_t tierpowresult_deserialize(const uint8_t *in, size_t in_size, TierPowResult *res) {
    if (!res || !in) return OP_NULL_PTR;
    if (in_size < TIER_POW_RESULT_SERIALIZED_SIZE) return OP_BUF_TOO_SMALL;

    size_t off = 0;
    res->tier = (Tier_t)in[off++];
    res->reserved[0] = in[off++];
    res->reserved[1] = in[off++];
    res->reserved[2] = in[off++];

    uint256_deserialize_be(in + off, UINT256_SIZE, &res->challenge.challenge);
    off += UINT256_SIZE;
    res->challenge.complexity = in[off++];
    deserialize_u64_be(in + off, &res->challenge.challenge_id, 8);
    off += 8;
    res->challenge.reserved[0] = in[off++];
    res->challenge.reserved[1] = in[off++];
    res->challenge.reserved[2] = in[off++];

    deserialize_u64_be(in + off, &res->solve.nonce, 8);
    off += 8;
    res->solve.complexity = in[off++];
    deserialize_u64_be(in + off, &res->solve.challenge_id, 8);
    off += 8;
    res->solve.reserved[0] = in[off++];
    res->solve.reserved[1] = in[off++];
    res->solve.reserved[2] = in[off++];

    uint64_t time_bits;
    deserialize_u64_be(in + off, &time_bits, 8);
    memcpy(&res->time_taken, &time_bits, 8);
    off += 8;

    return OP_SUCCESS;
}

static inline OpStatus_t tier_pow_verify_serialize_inputs(const tier_pow_challenge_t *pow,
                                                                   const tier_pow_solve_t *solve,
                                                                   uint8_t *out,
                                                                   size_t out_size)
{
    if (!pow || !solve || !out) return OP_NULL_PTR;
    if (out_size < (TIER_POW_CHALLENGE_SIZE + TIER_POW_SOLVE_SIZE)) return OP_BUF_TOO_SMALL;

    if (tier_pow_challenge_serialize(pow, out, TIER_POW_CHALLENGE_SIZE) != OP_SUCCESS) return OP_INVALID_INPUT;
    if (tier_pow_solve_serialize(solve, out + TIER_POW_CHALLENGE_SIZE, TIER_POW_SOLVE_SIZE) != OP_SUCCESS) return OP_INVALID_INPUT;
    return OP_SUCCESS;
}

static inline OpStatus_t tier_pow_verify_deserialize_inputs(const uint8_t *in,
                                                                     size_t in_size,
                                                                     tier_pow_challenge_t *pow,
                                                                     tier_pow_solve_t *solve)
{
    if (!pow || !solve || !in) return OP_NULL_PTR;
    if (in_size < (TIER_POW_CHALLENGE_SIZE + TIER_POW_SOLVE_SIZE)) return OP_BUF_TOO_SMALL;

    if (tier_pow_challenge_deserialize(in, TIER_POW_CHALLENGE_SIZE, pow) != OP_SUCCESS) return OP_INVALID_INPUT;
    if (tier_pow_solve_deserialize(in + TIER_POW_CHALLENGE_SIZE, TIER_POW_SOLVE_SIZE, solve) != OP_SUCCESS) return OP_INVALID_INPUT;
    return OP_SUCCESS;
}

static inline __attribute__((always_inline)) OpStatus_t minipowresult_serialize(const MiniPowResult *res, uint8_t *out, size_t out_size) {
    if (!res || !out) return OP_NULL_PTR;
    if (out_size < MINI_POW_RESULT_SERIALIZED_SIZE) return OP_BUF_TOO_SMALL;
    out[0] = (res->challengeid >> 24) & 0xFF;
    out[1] = (res->challengeid >> 16) & 0xFF;
    out[2] = (res->challengeid >> 8) & 0xFF;
    out[3] = res->challengeid & 0xFF;
    out[4] = (res->sessionid >> 24) & 0xFF;
    out[5] = (res->sessionid >> 16) & 0xFF;
    out[6] = (res->sessionid >> 8) & 0xFF;
    out[7] = res->sessionid & 0xFF;
    out[8] = res->tier;
    out[9] = res->isValid ? 1 : 0;
    out[10] = res->reserved[0];
    out[11] = res->reserved[1];
    return OP_SUCCESS;
}

static inline __attribute__((always_inline)) OpStatus_t minipowresult_deserialize(const uint8_t *in, size_t in_size, MiniPowResult *res) {
    if (!res || !in) return OP_NULL_PTR;
    if (in_size < MINI_POW_RESULT_SERIALIZED_SIZE) return OP_BUF_TOO_SMALL;
    res->challengeid = ((uint32_t)in[0] << 24) | ((uint32_t)in[1] << 16) | ((uint32_t)in[2] << 8) | in[3];
    res->sessionid = ((uint32_t)in[4] << 24) | ((uint32_t)in[5] << 16) | ((uint32_t)in[6] << 8) | in[7];
    res->tier = (Tier_t)in[8];
    res->isValid = in[9] != 0;
    res->reserved[0] = in[10];
    res->reserved[1] = in[11];
    return OP_SUCCESS;
}

#endif // UTIL_NETWORK_SERIALIZATION_H
