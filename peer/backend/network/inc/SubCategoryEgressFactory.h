#pragma once

#include "net/PacketCreationContext.h"
#include "protocol/routing/RoutingPacket.h"

class SubCategoryEgressFactory
{
public:

    RoutingPacket buildControl(PacketCreationContext pkc);
    RoutingPacket buildRouting(PacketCreationContext pkc);
    RoutingPacket buildProof(PacketCreationContext pkc);
    RoutingPacket buildSync(PacketCreationContext pkc);
};