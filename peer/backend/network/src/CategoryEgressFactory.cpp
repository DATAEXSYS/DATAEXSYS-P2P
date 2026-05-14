
#include "CategoryEgressFactory.h"

RoutingPacket CategoryEgressFactory::build(PacketCreationContext pkc)
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

RoutingPacket CategoryEgressFactory::buildControl(PacketCreationContext pkc)
{
    return opFactory.buildControl(pkc);
}

RoutingPacket CategoryEgressFactory::buildRouting(PacketCreationContext pkc)
{
    return opFactory.buildRouting(pkc);
}

RoutingPacket CategoryEgressFactory::buildProof(PacketCreationContext pkc)
{
    return opFactory.buildProof(pkc);
}

RoutingPacket CategoryEgressFactory::buildSync(PacketCreationContext pkc)
{
    return opFactory.buildSync(pkc);
}
