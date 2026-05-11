// =============================================================================
// test_network_adapter.cpp — NetworkAdapter unit tests
// =============================================================================

#include "NetworkAdapter.hpp"
#include <cassert>
#include <iostream>

using namespace dataexsys::adapters;

static void test_start_stop()
{
    NetworkAdapter na(0x0001);
    assert(na.start() == true);
    na.stop();
    std::cout << "[PASS] test_start_stop\n";
}

static void test_push_before_start_fails()
{
    NetworkAdapter na(0x0001);
    OwnedBuffer payload = {0xDE, 0xAD};
    auto msg = AdapterMessage::make(AdapterOpcode::NETWORK_PACKET_SEND,
                                    0x0002, 0x0001, std::move(payload));
    auto result = na.push(std::move(msg));
    assert(result.failed());
    assert(result.error == AdapterError::ADAPTER_NOT_STARTED);
    std::cout << "[PASS] test_push_before_start_fails\n";
}

static void test_poll_empty()
{
    NetworkAdapter na(0x0001);
    na.start();
    assert(na.poll() == std::nullopt);
    na.stop();
    std::cout << "[PASS] test_poll_empty\n";
}

static void test_ingest_raw_produces_message()
{
    NetworkAdapter na(0x0001);
    na.start();

    const uint8_t raw[] = {0x01, 0x02, 0x03};
    auto result = na.ingest_raw({raw, 3}, 0x0002);
    assert(result.ok);

    auto msg = na.poll();
    assert(msg.has_value());
    assert(msg->opcode == AdapterOpcode::NETWORK_PACKET_RECEIVED);
    assert(msg->payload.size() == 3);
    assert(msg->payload[0] == 0x01);

    na.stop();
    std::cout << "[PASS] test_ingest_raw_produces_message\n";
}

static void test_ingest_empty_payload_fails()
{
    NetworkAdapter na(0x0001);
    na.start();
    auto result = na.ingest_raw({}, 0x0002);
    assert(result.failed());
    assert(result.error == AdapterError::PAYLOAD_EMPTY);
    na.stop();
    std::cout << "[PASS] test_ingest_empty_payload_fails\n";
}

int main()
{
    test_start_stop();
    test_push_before_start_fails();
    test_poll_empty();
    test_ingest_raw_produces_message();
    test_ingest_empty_payload_fails();
    std::cout << "NetworkAdapter: all tests passed.\n";
    return 0;
}
