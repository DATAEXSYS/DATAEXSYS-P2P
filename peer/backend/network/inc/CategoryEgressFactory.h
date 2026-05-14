#pragma once

#include "protocol/routing/PacketCategory.h"
#include "protocol/routing/PacketHeader.h"
#include "protocol/routing/RoutingPacket.h"
#include "net/PacketCreationContext.h"
#include "SubCategoryEfressFactory.h"
class EgressCategoryFactory
{
public:

    RoutingPacket build(PacketCreationContext pkc);

private:

    RoutingPacket buildControl(PacketCreationContext pkc);
    RoutingPacket buildRouting(PacketCreationContext pkc);
    RoutingPacket buildProof(PacketCreationContext pkc);
    RoutingPacket buildSync(PacketCreationContext pkc);

    EgressSubCategoryFactory subFactory;
};