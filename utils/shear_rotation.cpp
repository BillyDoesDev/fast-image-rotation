/*
* HIGHLY EXPERIMENTAL
* WILL MOST LIKELY CAUSE FAULTY OUTPUTS
*
* TODO: Fix axis of rotation 
*/


#include <iostream>
#include <stdio.h>
#include "Halide.h"
#include "halide_image_io.h"
#include <cmath>


using namespace Halide;
using namespace Halide::Tools;

int main(int argc, char** argv) {
    // Load image using Halide
    Buffer<uint8_t> input = load_image("../assets/Lenna_test_image.png");

    // Rotation angle in degrees
    float rot_angle = 30;
    // Convert angle to radians
    float angle = rot_angle * M_PI / 180.0f;

    // Calculate shear factors
    float shear_x = -tan(angle / 2);
    float shear_y = sin(angle);
    float shear_x2 = -tan(angle / 2);

    // Define Halide variables
    Var x, y, c;

    // Intermediate buffers for shear transformations
    Func shear1("shear1"), shear2("shear2"), shear3("shear3");

    // Shear in x direction
    shear1(x, y, c) = input(clamp(cast<int>(x + shear_x * y), 0, input.width() - 1), y, c);

    // Shear in y direction
    Buffer<uint8_t> tmp1 = shear1.realize({input.width(), input.height(), input.channels()});
    shear2(x, y, c) = tmp1(x, clamp(cast<int>(y + shear_y * x), 0, input.height() - 1), c);

    // Shear in x direction again
    Buffer<uint8_t> tmp2 = shear2.realize({input.width(), input.height(), input.channels()});
    shear3(x, y, c) = tmp2(clamp(cast<int>(x + shear_x2 * y), 0, input.width() - 1), y, c);

    // Realize the function into an output buffer
    Buffer<uint8_t> output = shear3.realize({input.width(), input.height(), input.channels()});
    save_image(output, "../outputs/halide_shear_rotated.png");


    return 0;
}