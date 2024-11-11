#include <iostream>
#include <chrono>

class Timer {
public:
    // Starts the timer by recording the start time
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    // Stops the timer, calculates the elapsed time, and outputs it
    void stop() {
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;
        std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;
    }

private:
    std::chrono::high_resolution_clock::time_point start_time;
};

