// =============================================================================
// test_frontend_adapter.cpp — FrontendAdapter unit tests
// =============================================================================

#include "FrontendAdapter.hpp"
#include <cassert>
#include <iostream>
#include <string>

using namespace dataexsys::adapters;

static void test_start_stop()
{
    FrontendAdapter a(0x0040);
    assert(a.start());
    a.stop();
    std::cout << "[PASS] test_start_stop\n";
}

static void test_broadcast_valid_json()
{
    FrontendAdapter a(0x0040);
    a.start();
    auto r = a.broadcast_status(R"({"status":"ok","height":42})", 0x0001);
    assert(r.ok);

    auto msg = a.poll();
    assert(msg.has_value());
    assert(msg->opcode == AdapterOpcode::FRONTEND_STATUS_UPDATE);
    assert(!msg->payload.empty());

    a.stop();
    std::cout << "[PASS] test_broadcast_valid_json\n";
}

static void test_broadcast_invalid_json_fails()
{
    FrontendAdapter a(0x0040);
    a.start();
    // Not a JSON object — plain text
    auto r = a.broadcast_status("not json", 0x0001);
    assert(r.failed());
    assert(r.error == AdapterError::SERIALISATION_FAILED);
    a.stop();
    std::cout << "[PASS] test_broadcast_invalid_json_fails\n";
}

static void test_broadcast_before_start_fails()
{
    FrontendAdapter a(0x0040);
    auto r = a.broadcast_status(R"({"ok":true})", 0x0001);
    assert(r.failed());
    assert(r.error == AdapterError::ADAPTER_NOT_STARTED);
    std::cout << "[PASS] test_broadcast_before_start_fails\n";
}

static void test_poll_empty()
{
    FrontendAdapter a(0x0040);
    a.start();
    assert(a.poll() == std::nullopt);
    a.stop();
    std::cout << "[PASS] test_poll_empty\n";
}

int main()
{
    test_start_stop();
    test_broadcast_valid_json();
    test_broadcast_invalid_json_fails();
    test_broadcast_before_start_fails();
    test_poll_empty();
    std::cout << "FrontendAdapter: all tests passed.\n";
    return 0;
}
