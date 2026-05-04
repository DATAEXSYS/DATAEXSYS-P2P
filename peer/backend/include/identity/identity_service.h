#ifndef IDENTITY_SERVICE_H
#define IDENTITY_SERVICE_H

#include "identity.h"
#include "task_queue.h"

class IdentityService {
public:
    IdentityService(TaskQueue& queue);

    void initialize();   // explicitly schedule task

    NodeIdentity getIdentity() const;

private:
    TaskQueue& taskQueue;
    NodeIdentity identity;
    bool ready = false;

    static NodeIdentity issueNodeIdentity();
};

#endif