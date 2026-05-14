#pragma once

class IAdapter {
public:
    virtual ~IAdapter() = default;

    // lifecycle
    virtual void init() = 0;

    // telemetry only (read-only)
    virtual void tick() = 0;
};