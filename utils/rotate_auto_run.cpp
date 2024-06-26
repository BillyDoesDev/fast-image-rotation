#include "HalideBuffer.h"
#include "halide_benchmark.h"
#include "build/auto_schedule_false.h"
#include "build/auto_schedule_true.h"


#include <iostream>
#include <cstdlib>

int main() {
    const int width = 1024;
    const int height = 1024;
    const float angle = 0.5f;

    Halide::Runtime::Buffer<float> input(width, height);
    Halide::Runtime::Buffer<float> output(width, height);

    // Initialize input image
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            input(x, y) = rand() % 256;
        }
    }

    double auto_time = Halide::Tools::benchmark(5, 1, [&]() {
        // rotate(input, angle, output);
        auto_schedule_true(input, angle, output);
    });

    double ahh_time = Halide::Tools::benchmark(5, 1, [&]() {
        // rotate(input, angle, output);
        auto_schedule_false(input, angle, output);
    });

    // std::cout << "Time taken autoscheduling: " << auto_time * 1e3 << " milliseconds" << std::endl;
    // std::cout << "Time taken normally: " << ahh_time * 1e3 << " milliseconds" << std::endl;
    std::cout << auto_time * 1e3 << ", " << ahh_time * 1e3 << std::endl;

    return 0;
}
