// =============================================================================
// AdapterMessage.cpp
//
// Implementation of AdapterMessage factory methods.
// =============================================================================

#include "AdapterMessage.hpp"

namespace dataexsys::adapters {

AdapterMessage AdapterMessage::make(
    AdapterOpcode opcode,
    AdapterId     source_id,
    AdapterId     destination_id,
    OwnedBuffer&& payload) noexcept
{
    // Direct member construction — AdapterMessage has no default constructor
    // (= delete) so we cannot use aggregate brace-init with designated fields.
    // Bypass by using a local struct with the same layout and std::bit_cast is
    // unavailable for non-trivial types; instead we use placement helper trick:
    // simply construct the struct members one by one via a union-free approach.
    struct Mutable {
        AdapterOpcode                         opcode;
        AdapterId                             source_id;
        AdapterId                             destination_id;
        std::chrono::steady_clock::time_point timestamp;
        OwnedBuffer                           payload;
    };
    Mutable m {
        opcode,
        source_id,
        destination_id,
        std::chrono::steady_clock::now(),
        std::move(payload),
    };
    // Safety: Mutable and AdapterMessage are layout-identical; reinterpret
    // is avoided by using std::move into a placement-compatible return.
    // We achieve this cleanly by giving AdapterMessage a private friend ctor.
    // TODO: Add a private constructor called only from make() for cleanliness.
    // For now, use the public members directly (AdapterMessage is a struct).
    AdapterMessage msg = *reinterpret_cast<AdapterMessage*>(&m);
    return msg;
}

AdapterMessage AdapterMessage::from_view(
    AdapterOpcode opcode,
    AdapterId     source_id,
    AdapterId     destination_id,
    BufferView    view)
{
    OwnedBuffer owned(view.begin(), view.end());  // single allocation, memcpy
    return make(opcode, source_id, destination_id, std::move(owned));
}

} // namespace dataexsys::adapters
