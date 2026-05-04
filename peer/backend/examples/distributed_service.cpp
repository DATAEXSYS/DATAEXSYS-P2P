#include "runtime/runtime.h"
#include <iostream>
#include <vector>
#include <numeric>

using namespace runtime;

/**
 * @brief A high-performance data processing service.
 * 
 * Enforces the "Service Model" where work is only submitted and awaited.
 */
class AnalyticsService {
public:
    ResumableTask<double> calculate_risk_score(std::vector<int> data) {
        // Move execution to the runtime immediately
        co_await Runtime::schedule();

        std::cout << "[Service] Processing data on thread: " 
                  << std::this_thread::get_id() << std::endl;

        // Perform heavy calculation
        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        
        // Yield to allow other tasks to run if this is a long-running step
        co_await Yield{};

        double result = sum / data.size();
        
        std::cout << "[Service] Calculation complete: " << result << std::endl;
        co_return result;
    }

    void on_event(std::vector<int> event_data) {
        // Services never own threads, they just spawn tasks
        Runtime::instance().spawn(calculate_risk_score(std::move(event_data)));
    }
};

int main() {
    AnalyticsService service;
    
    std::cout << "[Main] Spawning tasks..." << std::endl;
    
    service.on_event({1, 2, 3, 4, 5});
    service.on_event({10, 20, 30});

    // Simulate main loop/server loop
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    std::cout << "[Main] Shutting down." << std::endl;
    return 0;
}
