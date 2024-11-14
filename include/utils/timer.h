#include <iostream>
#include <chrono>
#include <iomanip>

class Timer {
public:
    // Starts the timer by recording the start time
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    // Stops the timer, calculates the elapsed time, and outputs it
    void record() {
        num_frames++;
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;
        game_time += elapsed.count();
        frame_rate = game_time / num_frames;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "\rFrame Rate: " << 1.0f / frame_rate << " FPS    " << std::flush;
        start_time = end_time;
    }

    double game_time = 0.0f;
    double frame_rate = 0.0f;
    long int num_frames = 0;

private:
    std::chrono::high_resolution_clock::time_point start_time;
};

