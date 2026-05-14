#pragma once

#include "protocol/routing/PacketCategory.h"
#include "protocol/routing/PacketHeader.h"
#include "protocol/routing/RoutingPacket.h"
#include "net/PacketCreationContext.h"
#include "OperationEgressFactory.h"

class CategoryEgressFactory
{
public:

    RoutingPacket build(PacketCreationContext pkc);

private:

    RoutingPacket buildControl(PacketCreationContext pkc);
    RoutingPacket buildRouting(PacketCreationContext pkc);
    RoutingPacket buildProof(PacketCreationContext pkc);
    RoutingPacket buildSync(PacketCreationContext pkc);

    OperationEgressFactory opFactory;
};  