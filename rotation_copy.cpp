#include <iostream>
#include <stdio.h>
#include "Halide-17.0.1-x86-64-linux/include/Halide.h"
#include "./Halide-17.0.1-x86-64-linux/share/Halide/tools/halide_image_io.h"
#include <cmath>

using namespace std;
using namespace Halide;
using namespace Halide::Tools;

int main(int argc, char** argv) {
    // Load image using Halide
    Buffer<uint8_t> input = load_image("../Asset/test.png");

    // Rotation angle in degrees
    float rot_angle = 90;
    // Convert angle to radians
    float angle = rot_angle * M_PI / 180.0f;

    // Calculate center of the image
    float center_x = input.width() / 2.0f;
    float center_y = input.height() / 2.0f;

    // Define Halide variables
    Var x, y, c;

    // Define the rotation matrix
    Func rotate("rotate");
    Expr xf = cast<float>(x) - center_x;
    Expr yf = cast<float>(y) - center_y;
    Expr cos_angle = cos(angle);
    Expr sin_angle = sin(angle);
    Expr new_x = xf * cos_angle - yf * sin_angle + center_x;
    Expr new_y = xf * sin_angle + yf * cos_angle + center_y;

    // Perform bilinear interpolation
    Expr val = cast<uint8_t>(0);
    Expr xi = cast<int>(floor(new_x));
    Expr yi = cast<int>(floor(new_y));
    Expr xf1 = new_x - xi;
    Expr yf1 = new_y - yi;
    Expr xf2 = 1.0f - xf1;
    Expr yf2 = 1.0f - yf1;

    Expr clamped_x1 = clamp(xi, 0, input.width() - 1);
    Expr clamped_x2 = clamp(xi + 1, 0, input.width() - 1);
    Expr clamped_y1 = clamp(yi, 0, input.height() - 1);
    Expr clamped_y2 = clamp(yi + 1, 0, input.height() - 1);

    Expr top = xf2 * input(clamped_x1, clamped_y1, c) + xf1 * input(clamped_x2, clamped_y1, c);
    Expr bottom = xf2 * input(clamped_x1, clamped_y2, c) + xf1 * input(clamped_x2, clamped_y2, c);
    val = yf2 * top + yf1 * bottom;

    // Define the rotated function
    rotate(x, y, c) = cast<uint8_t>(val);

    // Realize the function into an output buffer
    Buffer<uint8_t> output = rotate.realize({input.width(), input.height(), input.channels()});
    save_image(output, "../Asset/rotated_image.png");

    std::cout << "Rotated image saved to ../Asset/rotated_image.png" << std::endl;

    return 0;
}