#ifndef CERTIFICATE_H
#define CERTIFICATE_H

#include "pkcertchain_config.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "datatype/uint256_t.h"
#include "datatype/uint512.h"

#include "util/Size_Offsets.h"
#include "util/To_BO_BE_Pimitives.h"
#include "util/To_BO_Def_Primitives.h"
#include "util/SignUtils.h"

#include "datatypes/OpStatus.h"
#include "datatypes/ipv6.h"

#define CERT_INLINE static inline __attribute__((always_inline))

/*
 * Serialized Layout (CERT_SIZE = 68 bytes)
 *
 *  Offset | Size | Field
 *  -------+------+----------------
 *    0    |  32  | pubSignKey
 *    32   |  32  | pubEncKey
 *    64   |  16  | ipv6 id
 *    80   |  3   | reserved
 *
 * NOTE:
 * Update CERT_SIZE if structure layout changes.
 */

typedef struct __attribute__((aligned(4))) {
    uint256 pubSignKey;      // 32 bytes
    uint256 pubEncKey;       // 32 bytes
    ipv6_t  id;              // 16 bytes
    uint8_t reserved[3];     // 3 bytes
} certificate;

/* =========================================================
 * INIT
 * ========================================================= */

CERT_INLINE void cert_init(certificate *cert)
{
    if (!cert) return;

    uint256_zero(&cert->pubSignKey);
    uint256_zero(&cert->pubEncKey);

    ipv6_zero(&cert->id);

    memset(cert->reserved, 0, sizeof(cert->reserved));
}

/* =========================================================
 * GETTERS
 * ========================================================= */

CERT_INLINE const uint256* cert_get_pubSignKey_ptr(const certificate *cert)
{
    return &cert->pubSignKey;
}

CERT_INLINE const uint256* cert_get_pubEncKey_ptr(const certificate *cert)
{
    return &cert->pubEncKey;
}

CERT_INLINE const ipv6_t* cert_get_id_ptr(const certificate *cert)
{
    return &cert->id;
}

/* =========================================================
 * SETTERS
 * ========================================================= */

CERT_INLINE void cert_set_pubSignKey(certificate *cert,
                                     const uint256 *key)
{
    cert->pubSignKey = *key;
}

CERT_INLINE void cert_set_pubEncKey(certificate *cert,
                                    const uint256 *key)
{
    cert->pubEncKey = *key;
}

CERT_INLINE void cert_set_id(certificate *cert,
                             const ipv6_t *id)
{
    cert->id = *id;
}

/* =========================================================
 * COPY
 * ========================================================= */

CERT_INLINE void cert_copy(certificate *dst,
                           const certificate *src)
{
    uint256_copy(&dst->pubSignKey, &src->pubSignKey);
    uint256_copy(&dst->pubEncKey, &src->pubEncKey);

    dst->id = src->id;

    memcpy(dst->reserved,
           src->reserved,
           sizeof(dst->reserved));
}

/* =========================================================
 * SERIALIZATION
 * NETWORK BYTE ORDER FORMAT
 * ========================================================= */

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

/* =========================================================
 * DESERIALIZATION
 * NETWORK -> HOST FORMAT
 * ========================================================= */

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

/* =========================================================
 * HASH
 * ========================================================= */

CERT_INLINE OpStatus_t hash_certificate(const certificate *cert,
                                        uint256 *out)
{
    if (!cert || !out)
        return OP_NULL_PTR;

    uint8_t buf[CERT_SIZE];

    OpStatus_t st =
        cert_serialize(cert, buf, sizeof(buf));

    if (st != OP_SUCCESS)
        return st;

    hash256_buffer(buf, sizeof(buf), out);

    return OP_SUCCESS;
}

/* =========================================================
 * SIGN
 * ========================================================= */

CERT_INLINE OpStatus_t cert_sign(const certificate *cert,
                                 const uint256 *priv_key,
                                 uint512 *out_sig)
{
    if (!cert || !priv_key || !out_sig)
        return OP_NULL_PTR;

    uint8_t buf[CERT_SIZE];

    OpStatus_t st =
        cert_serialize(cert, buf, sizeof(buf));

    if (st != OP_SUCCESS)
        return st;

    return sign_buffer_ed25519(buf,
                               sizeof(buf),
                               priv_key,
                               out_sig);
}

#endif // CERTIFICATE_H