// =============================================================================
// test_pkcertchain_adapter.cpp — PKCertChainAdapter unit tests
// =============================================================================

#include "PKCertChainAdapter.hpp"
#include <cassert>
#include <iostream>

using namespace dataexsys::adapters;

static void test_start_stop()
{
    PKCertChainAdapter a(0x0010);
    assert(a.start());
    a.stop();
    std::cout << "[PASS] test_start_stop\n";
}

static void test_push_before_start_fails()
{
    PKCertChainAdapter a(0x0010);
    OwnedBuffer payload = {0x00};
    auto msg = AdapterMessage::make(AdapterOpcode::PKC_NEW_BLOCK,
                                    0x0001, 0x0010, std::move(payload));
    auto r = a.push(std::move(msg));
    assert(r.failed());
    assert(r.error == AdapterError::ADAPTER_NOT_STARTED);
    std::cout << "[PASS] test_push_before_start_fails\n";
}

static void test_unsupported_opcode_rejected()
{
    PKCertChainAdapter a(0x0010);
    a.start();
    OwnedBuffer payload = {0xFF};
    auto msg = AdapterMessage::make(AdapterOpcode::NETWORK_PACKET_RECEIVED,
                                    0x0001, 0x0010, std::move(payload));
    auto r = a.push(std::move(msg));
    assert(r.failed());
    assert(r.error == AdapterError::UNSUPPORTED_OPCODE);
    a.stop();
    std::cout << "[PASS] test_unsupported_opcode_rejected\n";
}

static void test_submit_new_block_empty_fails()
{
    PKCertChainAdapter a(0x0010);
    a.start();
    auto r = a.submit_new_block({}, 0x0001);
    assert(r.failed());
    assert(r.error == AdapterError::PAYLOAD_EMPTY);
    a.stop();
    std::cout << "[PASS] test_submit_new_block_empty_fails\n";
}

int main()
{
    test_start_stop();
    test_push_before_start_fails();
    test_unsupported_opcode_rejected();
    test_submit_new_block_empty_fails();
    std::cout << "PKCertChainAdapter: all tests passed.\n";
    return 0;
}
