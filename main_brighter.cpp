#include <iostream>
#include <stdio.h>
#include "Halide-17.0.1-x86-64-linux/include/Halide.h"
#include "./Halide-17.0.1-x86-64-linux/share/Halide/tools/halide_image_io.h"
#include <cmath>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


using namespace Halide::Tools;

int main(int argc, char **argv) {
    // Load image using Halide
    Halide::Buffer<uint8_t> input = load_image("../Asset/test.png");

    // Define the Halide function for brightening the image
    Halide::Func brighter;
    Halide::Var x, y, c;
    Halide::Expr value = input(x, y, c);
    value = Halide::cast<float>(value);
    value = value * 1.5f;
    value = Halide::min(value, 255.0f);
    value = Halide::cast<uint8_t>(value);
    brighter(x, y, c) = value;

    // Realize the function into an output buffer
    Halide::Buffer<uint8_t> output = brighter.realize({input.width(), input.height(), input.channels()});
    save_image(output, "../Asset/brighter_image.png");

    printf("Success!\n");
    return 0;
}
