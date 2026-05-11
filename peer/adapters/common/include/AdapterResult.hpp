#pragma once

// =============================================================================
// AdapterResult.hpp
//
// Return value type for all IAdapter operations.
//
// RULE: Every adapter method that can fail MUST return AdapterResult.
//       Never return raw bool, int, or throw exceptions across adapter boundaries.
// =============================================================================

#include "AdapterErrors.hpp"

#include <optional>
#include <string>

namespace dataexsys::adapters {

struct AdapterResult {

    bool                       ok;          ///< true = success
    AdapterError               error;       ///< NONE when ok == true
    std::optional<std::string> diagnostic;  ///< Human-readable context, optional

    // -----------------------------------------------------------------------
    // Factory helpers
    // -----------------------------------------------------------------------

    /// Construct a successful result.
    [[nodiscard]]
    static AdapterResult success() noexcept
    {
        return { true, AdapterError::NONE, std::nullopt };
    }

    /// Construct a failure result with mandatory error code.
    [[nodiscard]]
    static AdapterResult fail(
        AdapterError               error,
        std::optional<std::string> diagnostic = std::nullopt) noexcept
    {
        return { false, error, std::move(diagnostic) };
    }

    // -----------------------------------------------------------------------
    // Convenience predicates
    // -----------------------------------------------------------------------

    [[nodiscard]] explicit operator bool() const noexcept { return ok; }

    [[nodiscard]] bool failed() const noexcept { return !ok; }
};

} // namespace dataexsys::adapters
