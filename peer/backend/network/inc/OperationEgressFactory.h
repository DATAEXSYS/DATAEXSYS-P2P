#pragma once

#include "net/PacketCreationContext.h"
#include "protocol/routing/RoutingPacket.h"
#include "StatusEgressCateogry.h"

class OperationEgressFactory
{
    StatusEgressFactory statusFactory;
    
public:

    RoutingPacket buildControl(PacketCreationContext pkc);
    RoutingPacket buildRouting(PacketCreationContext pkc);
    RoutingPacket buildProof(PacketCreationContext pkc);
    RoutingPacket buildSync(PacketCreationContext pkc);
};