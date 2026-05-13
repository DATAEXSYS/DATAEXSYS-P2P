#ifndef TRUST_NODE_H
#define TRUST_NODE_H

#if !defined(__linux__)
#error "This project is optimized for Linux systems only"
#endif

#include <stdint.h>
#include <string.h>
#include <shared/inc/datatypes/ipv6.h>
#include <arpa/inet.h>   // htonl, ntohl


#define TRUST_SCALE UINT32_MAX
#define TRUST_NODE_INLINE inline __attribute__((always_inline))

typedef struct __attribute__((aligned(4))) {
    ipv6_t nodeid;  //128 bits for IPv6 address
    uint32_t trust; // 32-bit trust score
    uint32_t acks; // 32 bits
    uint32_t nacks; // 32 bits
} TrustNode;

/* -------------------------
   INIT
--------------------------*/

TRUST_NODE_INLINE void trust_node_init(TrustNode *node)
{
    ipv6_init(&node->nodeid, (uint8_t[16]){0});
    node->trust = 20;
    node->acks = 0;
    node->nacks = 0;
}

/* -------------------------
   GETTERS
--------------------------*/

TRUST_NODE_INLINE ipv6_t trust_node_get_id(const TrustNode *node)
{
    return node->nodeid;
}

TRUST_NODE_INLINE const uint32_t* trust_node_get_trust(const TrustNode *node)
{
    return &node->trust;
}

TRUST_NODE_INLINE const uint32_t* trust_node_get_acks(const TrustNode *node)
{
    return &node->acks;
}

TRUST_NODE_INLINE const uint32_t* trust_node_get_nacks(const TrustNode *node)
{
    return &node->nacks;
}

/* -------------------------
   SETTERS
--------------------------*/

TRUST_NODE_INLINE void trust_node_set_id(TrustNode *node, ipv6_t id)
{
    node->nodeid = id;
}

TRUST_NODE_INLINE void trust_node_set_trust(TrustNode *node, uint32_t trust)
{
    node->trust = trust;
}

/* -------------------------
   INCREMENT OPERATIONS
--------------------------*/

TRUST_NODE_INLINE void trust_node_inc_ack(TrustNode *node)
{
    node->acks++;
}

TRUST_NODE_INLINE void trust_node_inc_nack(TrustNode *node)
{
    node->nacks++;
}

/* -------------------------
   TRUST UPDATE LOGIC
--------------------------*/
/*
    Simple model:
    - ACK increases trust
    - NACK decreases trust
    - bounded to avoid overflow/underflow
*/

TRUST_NODE_INLINE void trust_node_update_trust(TrustNode *node)
{
    uint64_t a = node->acks;
    uint64_t n = node->nacks;

    uint64_t total = a + n + 1;

    // scaled Bayesian probability (fixed-point Q32)
    uint64_t trust_scaled = (a * TRUST_SCALE) / total;

    node->trust = (uint32_t)trust_scaled;

    node->acks = 0;
    node->nacks = 0;
}

TRUST_NODE_INLINE int trust_node_serialize(const TrustNode *node,
                                           uint8_t *out,
                                           size_t out_size)
{
    if (!node || !out)
        return -1;

    if (out_size < TRUST_NODE_SIZE)
        return -2;

    size_t offset = 0;

    /* IPv6 (16 bytes) */
    ipv6_serialize(&node->nodeid, out + offset, IPV6_SIZE);
    offset += IPV6_SIZE;

    /* trust -> network byte order */
    uint32_t trust_net = htonl(node->trust);
    memcpy(out + offset, &trust_net, UINT32_SIZE);
    offset += UINT32_SIZE;

    /* acks -> network byte order */
    uint32_t acks_net = htonl(node->acks);
    memcpy(out + offset, &acks_net, UINT32_SIZE);
    offset += UINT32_SIZE;

    /* nacks -> network byte order */
    uint32_t nacks_net = htonl(node->nacks);
    memcpy(out + offset, &nacks_net, UINT32_SIZE);

    return 0;
}

TRUST_NODE_INLINE int trust_node_deserialize(const uint8_t *in,
                                             size_t in_size,
                                             TrustNode *node)
{
    if (!node || !in)
        return -1;

    if (in_size < TRUST_NODE_SIZE)
        return -2;

    size_t offset = 0;

    /* IPv6 */
    ipv6_deserialize(in + offset, IPV6_SIZE, &node->nodeid);
    offset += IPV6_SIZE;

    /* trust -> host byte order */
    uint32_t trust_net;
    memcpy(&trust_net, in + offset, UINT32_SIZE);
    node->trust = ntohl(trust_net);
    offset += UINT32_SIZE;

    /* acks -> host byte order */
    uint32_t acks_net;
    memcpy(&acks_net, in + offset, UINT32_SIZE);
    node->acks = ntohl(acks_net);
    offset += UINT32_SIZE;

    /* nacks -> host byte order */
    uint32_t nacks_net;
    memcpy(&nacks_net, in + offset, UINT32_SIZE);
    node->nacks = ntohl(nacks_net);

    return 0;
}

#endif // TRUST_NODE_H  