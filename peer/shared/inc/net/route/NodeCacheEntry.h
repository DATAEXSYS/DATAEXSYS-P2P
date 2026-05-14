#pragma once

#include "core/datatypes/ipv6.h"
typedef struct {

    ipv6_t destination_node;

    ipv6_t neighborNode;

    bool reachable;

} NodeCacheEntry;