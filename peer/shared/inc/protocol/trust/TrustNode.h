#pragma once

#include <stdint.h>
#include "core/datatypes/ipv6.h"

#ifdef __linux__
#include <stdbool.h>
#else
#error "This project is optimized for Linux systems only"
#endif

// =================================================
// TrustNode (Core State Object)
// =================================================
// This struct stores:
// - Identity (nodeid)
// - Bayesian belief (p)
// - Evidence counters (acks/nacks)
// - Strength (s = evidence weight / confidence)
// =================================================

typedef struct __attribute__((aligned(4))) {

    // -------------------------------------------------
    // Identity
    // -------------------------------------------------
    ipv6_t nodeid;

    // -------------------------------------------------
    // Bayesian Trust State
    // p in [0, UINT32_MAX] scaled probability
    // -------------------------------------------------
    uint32_t trust;

    // -------------------------------------------------
    // Evidence counters (current window)
    // -------------------------------------------------
    uint32_t acks;
    uint32_t nacks;

    // -------------------------------------------------
    // Bayesian strength (s)
    // how much history influences trust
    // -------------------------------------------------
    uint32_t strength;

} TrustNode;