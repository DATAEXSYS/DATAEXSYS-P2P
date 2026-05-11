// =============================================================================
// test_rolling_signatures_adapter.cpp — RollingSignaturesAdapter unit tests
// =============================================================================

#include "RollingSignaturesAdapter.hpp"
#include <cassert>
#include <iostream>

using namespace dataexsys::adapters;

static void test_start_stop()
{
    RollingSignaturesAdapter a(0x0020);
    assert(a.start());
    a.stop();
    std::cout << "[PASS] test_start_stop\n";
}

static void test_push_before_start_fails()
{
    RollingSignaturesAdapter a(0x0020);
    OwnedBuffer payload = {0x00};
    auto msg = AdapterMessage::make(AdapterOpcode::ROLLING_VERIFY_PACKET,
                                    0x0001, 0x0020, std::move(payload));
    auto r = a.push(std::move(msg));
    assert(r.failed());
    assert(r.error == AdapterError::ADAPTER_NOT_STARTED);
    std::cout << "[PASS] test_push_before_start_fails\n";
}

static void test_submit_verify_empty_fails()
{
    RollingSignaturesAdapter a(0x0020);
    a.start();
    auto r = a.submit_verify({}, 0x0001);
    assert(r.failed());
    assert(r.error == AdapterError::PAYLOAD_EMPTY);
    a.stop();
    std::cout << "[PASS] test_submit_verify_empty_fails\n";
}

static void test_submit_forward_empty_fails()
{
    RollingSignaturesAdapter a(0x0020);
    a.start();
    auto r = a.submit_forward({}, 0x11, 0x0001);
    assert(r.failed());
    assert(r.error == AdapterError::PAYLOAD_EMPTY);
    a.stop();
    std::cout << "[PASS] test_submit_forward_empty_fails\n";
}

int main()
{
    test_start_stop();
    test_push_before_start_fails();
    test_submit_verify_empty_fails();
    test_submit_forward_empty_fails();
    std::cout << "RollingSignaturesAdapter: all tests passed.\n";
    return 0;
}
