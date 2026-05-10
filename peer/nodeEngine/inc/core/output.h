#pragma once

#include <utility>
#include <stdexcept>
#include "core/types/nooutput.h"

template<typename T>
class Output {
private:
    T value{};
    bool has_value = false;

public:
    Output() = default;

    Output(T v)
        : value(std::move(v)), has_value(true) {}

    bool hasValue() const noexcept {
        return has_value;
    }

    const T& getValue() const {
        if (!has_value) {
            throw std::runtime_error("Output has no value");
        }
        return value;
    }

    T& getValue() {
        if (!has_value) {
            throw std::runtime_error("Output has no value");
        }
        return value;
    }
};

// --------------------
// Specialization: NoOutput
// --------------------
template<>
class Output<NoOutput> {
public:
    Output() = default;

    // no storage, no value, no state

    bool hasValue() const noexcept {
        return true; // always "valid" because there's nothing to validate
    }

    void getValue() const noexcept {
        // no-op: intentionally does nothing
    }
};