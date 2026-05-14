#pragma once


class StatusEgressFactory{


public:
    RoutingPacket build(PacketCreationContext pkc);
};