#ifndef TRUST_NODE_H
#define TRUST_NODE_H

#if !defined(__linux__)
#error "This project is optimized for Linux systems only"
#endif

#include <cstdint>
#include <stdio.h>
#include <string.h>

#define TRUST_NODE_INLINE inline __attribute__((always_inline))


typedef struct __attribute__((aligned(32))) {
    uint8_t nodeid;
    uint32_t trust;
    uint32_t acks;
    uint32_t nacks;
    uint8_t reserved[19];
} TrustNode;

TRUST_NODE_INLINE void trust_node_init(TrustNode *&node){
    node->nodeid = 0;
    node->trust = 20;
    node->acks = 0;
    node->nacks = 0;
    memset(node->reserved, 0, sizeof(node->reserved));
}

TRUST_NODE_INLINE const uint8_t trust_node_get_id(const TrustNode *&node){
    return node->nodeid;
}

TRUST_NODE_INLINE const uint32_t* trust_node_get_trust(const TrustNode *&node){
    return &node->trust;
}

TRUST_NODE_INLINE const uint32_t* trust_node_get_acks(const TrustNode *&node){
    return &node->acks;
}

TRUST_NODE_INLINE const uint32_t* trust_node_get_nacks(const TrustNode *&node){
    return &node->nacks;
}

TRUST_NODE_INLINE void trust_node_set_id(TrustNode *&node, const uint8_t address){
    node->nodeid = address;
}

TRUST_NODE_INLINE void trust_node_set_trust(TrustNode *&node, const uint32_t trust){
    node->trust = trust; 
}
TRUST_NODE_INLINE void trust_node_set_acks(TrustNode *&node, const uint32_t acks){
    node->acks = acks; 
}
TRUST_NODE_INLINE void trust_node_set_nacks(TrustNode *&node, const uint32_t nacks){
    node->nacks = nacks; 
}

TRUST_NODE_INLINE void trust_node_update_trust(TrustNode*&node, const uint8_t ack_type){

}



#endif // TRUST_NODE_H