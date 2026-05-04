#include<iostream>
#include "task/task_queue.h"
#include "identity/identity_service.h"
using namespace std;


int main(){
    TaskQueue taskQueue;
    IdentityService identityService(taskQueue);
    identityService.initialize();

    // Keep the main thread alive to allow background tasks to run
    while (true) {
        this_thread::sleep_for(chrono::seconds(1));
    }

    return 0;
}