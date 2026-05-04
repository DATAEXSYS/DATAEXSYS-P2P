#ifndef IDENTITY_SERVICE_H
#define IDENTITY_SERVICE_H

#include "identity.h"
#include "task/task_queue.h"
#include <cstdint>

#include <mutex>

class IdentityService {
public:
    explicit IdentityService(TaskQueue& queue);

    // schedule async identity generation
    void initialize();

    // retrieve generated identity
    NodeIdentity getIdentity() const;

    // state check
    bool isReady() const;

private:
    TaskQueue& taskQueue;

    mutable std::mutex mtx;

    NodeIdentity identity;

    bool ready = false;

private:
    static NodeIdentity issueNodeIdentity();
};

#endif