#include "OperationEgressFactory.h"

RoutingPacket OperationEgressFactory::buildControl(PacketCreationContext pkc)
{
    switch (pkc.operation)
    {
        case PACKET_OP_PING:
            return ping(pkc);

        case PACKET_OP_PONG:
            return pong(pkc);

        case PACKET_OP_HEARTBEAT:
            return heartbeat(pkc);

        default:
            return {}; // invalid packet
    }
}

RoutingPacket OperationEgressFactory::buildRouting(PacketCreationContext pkc)
{
    switch (pkc.operation)
    {
        case PACKET_OP_DSR_RREQ:
            return dsr_rreq(pkc);

        case PACKET_OP_DSR_RREP:
            return dsr_rrep(pkc);

        case PACKET_OP_DSR_RERR:
            return dsr_rerr(pkc);

        case PACKET_OP_DSR_DATA:
            return dsr_data(pkc);

        default:
            return {}; // invalid packet
    }
}

RoutingPacket OperationEgressFactory::buildProof(PacketCreationContext pkc)
{
    switch (pkc.operation)
    {
        case PACKET_OP_MINIPOW_CHALLENGE:
            return mini_pow_challenge(pkc);

        case PACKET_OP_MINIPOW_RESULT:
            return mini_pow_result(pkc);

        case PACKET_OP_TIERPOW_CHALLENGE:
            return tier_pow_challenge(pkc);

        case PACKET_OP_TIERPOW_RESULT:
            return tier_pow_result(pkc);

        default:
            return {}; // invalid packet
    }
}

RoutingPacket OperationEgressFactory::buildSync(PacketCreationContext pkc)
{
    switch (pkc.operation)
    {
        case PACKET_OP_PKC_SYNC:
            return pkc_sync(pkc);

        case PACKET_OP_LTD_SYNC:
            return ltd_sync(pkc);

        case PACKET_OP_RTC_SYNC:
            return rtc_sync(pkc);

        default:
            return {}; // invalid packet
    }
}

RoutingPacket OperationEgressFactory::dsr_rreq(PacketCreationContext pkc) {return statusFactory.dsr_rreq(pkc)}
RoutingPacket OperationEgressFactory::dsr_rrep(PacketCreationContext pkc) {return statusFactory.dsr_rrep(pkc)}
RoutingPacket OperationEgressFactory::dsr_rerr(PacketCreationContext pkc) {return statusFactory.dsr_rerr(pkc)}
RoutingPacket OperationEgressFactory::dsr_data(PacketCreationContext pkc) {return statusFactory.dsr_data(pkc)}

RoutingPacket OperationEgressFactory::mini_pow_challenge(PacketCreationContext pkc) {return statusFactory.mini_pow_challenge(pkc)}
RoutingPacket OperationEgressFactory::mini_pow_result(PacketCreationContext pkc) {return statusFactory.mini_pow_result(pkc)}
RoutingPacket OperationEgressFactory::tier_pow_challenge(PacketCreationContext pkc) {return statusFactory.tier_pow_challenge(pkc)}
RoutingPacket OperationEgressFactory::tier_pow_result(PacketCreationContext pkc) {return statusFactory.tier_pow_result(pkc)}

RoutingPacket OperationEgressFactory::pkc_sync(PacketCreationContext pkc) {return statusFactory.pkc_sync(pkc)}
RoutingPacket OperationEgressFactory::ltd_sync(PacketCreationContext pkc) {return statusFactory.ltd_sync(pkc)}
RoutingPacket OperationEgressFactory::rtc_sync(PacketCreationContext pkc) {return statusFactory.rtc_sync(pkc)}

RoutingPacket OperationEgressFactory::ping(PacketCreationContext pkc) {return statusFactory.ping(pkc)}
RoutingPacket OperationEgressFactory::pong(PacketCreationContext pkc) {return statusFactory.pong(pkc)}
RoutingPacket OperationEgressFactory::heartbeat(PacketCreationContext pkc) {return statusFactory.heartbeat(pkc)}