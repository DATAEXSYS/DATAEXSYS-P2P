#include <iostream>
#include <string>
#include <tuple>
#include "runtime/tasksystem.h"

int main() {
    TaskSystem system;
    system.start();

    std::cout << "Submitting task..." << std::endl;

    // RULE 10: Abstracted std::move logic and Task creation
    // User provides raw input and a typed lambda
    auto handle = system.submit(std::make_tuple(3, 5, 6), [](std::tuple<int, int, int> input) {
        auto [a, b, c] = input;
        std::cout << "Executing task: " << a << " * " << b << " * " << c << std::endl;
        return a * b * c;
    });

    // RULE 4: TaskHandle is the only typed access point
    try {
        int result = handle.get<int>();
        std::cout << "Result received: " << result << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // Submit a task without input
    auto handle2 = system.submit([]() {
        return std::string("Hello from background task!");
    });

    std::cout << "Result 2: " << handle2.get<std::string>() << std::endl;

    system.stop();
    std::cout << "System stopped." << std::endl;

    return 0;
}
