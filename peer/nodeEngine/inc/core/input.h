#pragma once

#include <utility>
#include "core/types/noinput.h"

// Generic case
template<typename T>
class Input {
private:
    T value{};

public:
    Input() = default;

    Input(T v)
        : value(std::move(v)) {}

    const T& get() const noexcept {
        return value;
    }

    T& get() noexcept {
        return value;
    }

    void set(T v) {
        value = std::move(v);
    }
};


template<>
class Input<NoInput> {
public:
    Input() = default;

    // no data stored at all

    void get() const noexcept {}
};