// =============================================================================
// test_local_trust_diaries_adapter.cpp — LocalTrustDiariesAdapter unit tests
// =============================================================================

#include "LocalTrustDiariesAdapter.hpp"
#include <cassert>
#include <iostream>

using namespace dataexsys::adapters;

static void test_start_stop()
{
    LocalTrustDiariesAdapter a(0x0030);
    assert(a.start());
    a.stop();
    std::cout << "[PASS] test_start_stop\n";
}

static void test_lookup_unknown_returns_nullopt()
{
    LocalTrustDiariesAdapter a(0x0030);
    a.start();
    assert(a.lookup(0x42) == std::nullopt);
    a.stop();
    std::cout << "[PASS] test_lookup_unknown_returns_nullopt\n";
}

static void test_ack_creates_entry()
{
    LocalTrustDiariesAdapter a(0x0030);
    a.start();
    auto r = a.record_ack(0x01);
    assert(r.ok);
    auto snap = a.lookup(0x01);
    assert(snap.has_value());
    assert(snap->acks == 1);
    assert(snap->trust == 20);  // default initial trust
    a.stop();
    std::cout << "[PASS] test_ack_creates_entry\n";
}

static void test_nack_increments_nacks()
{
    LocalTrustDiariesAdapter a(0x0030);
    a.start();
    a.record_nack(0x02);
    a.record_nack(0x02);
    auto snap = a.lookup(0x02);
    assert(snap.has_value());
    assert(snap->nacks == 2);
    a.stop();
    std::cout << "[PASS] test_nack_increments_nacks\n";
}

static void test_trust_snapshot_roundtrip()
{
    TrustSnapshot original { 0x05, 42, 10, 3 };
    auto buf  = original.serialise();
    assert(buf.size() == 13);
    auto decoded = TrustSnapshot::deserialise(buf);
    assert(decoded.has_value());
    assert(decoded->node_id == 0x05);
    assert(decoded->trust   == 42);
    assert(decoded->acks    == 10);
    assert(decoded->nacks   == 3);
    std::cout << "[PASS] test_trust_snapshot_roundtrip\n";
}

static void test_push_before_start_fails()
{
    LocalTrustDiariesAdapter a(0x0030);
    OwnedBuffer payload = {0x01};
    auto msg = AdapterMessage::make(AdapterOpcode::TRUST_ACK,
                                    0x0001, 0x0030, std::move(payload));
    auto r = a.push(std::move(msg));
    assert(r.failed());
    assert(r.error == AdapterError::ADAPTER_NOT_STARTED);
    std::cout << "[PASS] test_push_before_start_fails\n";
}

int main()
{
    test_start_stop();
    test_lookup_unknown_returns_nullopt();
    test_ack_creates_entry();
    test_nack_increments_nacks();
    test_trust_snapshot_roundtrip();
    test_push_before_start_fails();
    std::cout << "LocalTrustDiariesAdapter: all tests passed.\n";
    return 0;
}
