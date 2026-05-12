#ifndef TRUST_NODE_H
#define TRUST_NODE_H

#if !defined(__linux__)
#error "This project is optimized for Linux systems only"
#endif

#include <stdint.h>
#include <string.h>
#include <shared/inc/datatypes/ipv6.h>

#define TRUST_SCALE UINT32_MAX
#define TRUST_NODE_INLINE inline __attribute__((always_inline))

typedef struct __attribute__((aligned(32))) {
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


#endif // TRUST_NODE_H