
#include "CategoryEgressFactory.h"

RoutingPacket EgressCategoryFactory::build(PacketCreationContext pkc)
{
    switch (pkc.category)
    {
        case PACKET_CATEGORY_CONTROL:
            return buildControl(pkc);

        case PACKET_CATEGORY_ROUTING:
            return buildRouting(pkc);

        case PACKET_CATEGORY_PROOF:
            return buildProof(pkc);

        case PACKET_CATEGORY_SYNC:
            return buildSync(pkc);

        default:
            return RoutingPacket{};
    }
}

RoutingPacket EgressCategoryFactory::buildControl(PacketCreationContext pkc)
{
    return subFactory.buildControl(pkc);
}

RoutingPacket EgressCategoryFactory::buildRouting(PacketCreationContext pkc)
{
    return subFactory.buildRouting(pkc);
}

RoutingPacket EgressCategoryFactory::buildProof(PacketCreationContext pkc)
{
    return subFactory.buildProof(pkc);
}

RoutingPacket EgressCategoryFactory::buildSync(PacketCreationContext pkc)
{
    return subFactory.buildSync(pkc);
}
