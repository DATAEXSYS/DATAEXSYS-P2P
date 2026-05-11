#pragma once

// =============================================================================
// BufferView.hpp
//
// Lightweight, immutable, non-owning byte buffer abstraction.
//
// RULES:
//  - Adapters MUST use BufferView for zero-copy reads of inbound data.
//  - Adapters MUST use OwnedBuffer for data that must outlive its source.
//  - NEVER store a BufferView beyond the lifetime of the backing allocation.
//  - NEVER pass BufferView across thread boundaries without synchronisation.
// =============================================================================

#include <cstdint>
#include <cstddef>
#include <span>
#include <vector>

namespace dataexsys::adapters {

/// Non-owning, read-only view into a contiguous byte sequence.
/// Modelled as std::span<const uint8_t> — no allocation, no copy.
using BufferView = std::span<const uint8_t>;

/// Owning heap-allocated byte buffer used inside AdapterMessage payloads.
using OwnedBuffer = std::vector<uint8_t>;

// ---------------------------------------------------------------------------
// BufferView construction helpers
// ---------------------------------------------------------------------------

/// Create a BufferView from a raw pointer + length.
/// Caller guarantees [data, data+len) is valid for the view's lifetime.
[[nodiscard]]
inline BufferView make_view(const uint8_t* data, std::size_t len) noexcept
{
    return { data, len };
}

/// Create a BufferView from an OwnedBuffer (zero-copy).
[[nodiscard]]
inline BufferView make_view(const OwnedBuffer& buf) noexcept
{
    return { buf.data(), buf.size() };
}

} // namespace dataexsys::adapters
