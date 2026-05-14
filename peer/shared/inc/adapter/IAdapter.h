#pragma once

#include "runtime/tasksystem.h"

class IAdapter {
protected:
    TaskSystem* taskSystem = nullptr;

public:
    virtual ~IAdapter() = default;

    // inject dependency once (preferred over constructor chaos in interfaces)
    void bindTaskSystem(TaskSystem* system)
    {
        taskSystem = system;
    }

    TaskSystem* getTaskSystem() const
    {
        return taskSystem;
    }

    // lifecycle
    virtual void init() = 0;

    // telemetry only (read-only, no side effects required)
    virtual void tick() = 0;
};