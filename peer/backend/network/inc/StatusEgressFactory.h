#pragma once


class StatusEgressFactory{



public:
    RoutingPacket dsr_rreq(PacketCreationContext pkc);
    RoutingPacket dsr_rrep(PacketCreationContext pkc);
    RoutingPacket dsr_rerr(PacketCreationContext pkc);
    RoutingPacket dsr_data(PacketCreationContext pkc);  

    RoutingPacket mini_pow_challenge(PacketCreationContext pkc);
    RoutingPacket mini_pow_result(PacketCreationContext pkc);
    RoutingPacket tier_pow_challenge(PacketCreationContext pkc);
    RoutingPacket tier_pow_result(PacketCreationContext pkc);

    RoutingPacket pkc_sync(PacketCreationContext pkc);
    RoutingPacket ltd_sync(PacketCreationContext pkc);
    RoutingPacket rtc_sync(PacketCreationContext pkc);

    RoutingPacket ping(PacketCreationContext pkc);
    RoutingPacket pong(PacketCreationContext pkc);
    RoutingPacket heartbeat(PacketCreationContext pkc);


};