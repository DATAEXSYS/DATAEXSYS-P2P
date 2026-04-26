#ifndef IDENTITY_SERVICE_H
#define IDENTITY_SERVICE_H

#include "identity.h"

class IdentityService {
public:
    static NodeIdentity issueNodeIdentity();
};

#endif