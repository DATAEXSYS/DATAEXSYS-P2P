#include <iostream>
#include <thread>

#include "task/task_queue.h"
#include "identity/identity_service.h"

int main() {

    TaskQueue taskQueue(2);

    IdentityService identityService(taskQueue);

    identityService.initialize();

    while (!identityService.isReady()) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100)
        );
    }

    NodeIdentity identity =
        identityService.getIdentity();

    std::cout << "Identity generated\n";
    std::cout << "IPv6: "
              << identity.ipv6
              << std::endl;

    return 0;
}