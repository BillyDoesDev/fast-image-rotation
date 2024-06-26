#include "Halide.h"
#include "halide_image_io.h"
#include <cmath>

using namespace Halide;
using namespace Halide::Tools;

int main() {
    // Load input image
    Buffer<uint8_t> input = load_image("input.png");

    // Define variables
    Var x("x"), y("y"), c("c");

    // Define the angle of rotation (in radians)
    float angle = 45.0f * (M_PI / 180.0f); // converting degrees to radians

    // Define the center of rotation
    float center_x = input.width() / 2.0f;
    float center_y = input.height() / 2.0f;

    // Define the rotation matrix elements
    float cos_angle = std::cos(angle);
    float sin_angle = std::sin(angle);

    // Define the forward mapping function
    Func forward_map("forward_map");
    forward_map(x, y) = Tuple(cast<float>(center_x) + (cast<float>(x) - cast<float>(center_x)) * cos_angle - (cast<float>(y) - cast<float>(center_y)) * sin_angle,
                              cast<float>(center_y) + (cast<float>(x) - cast<float>(center_x)) * sin_angle + (cast<float>(y) - cast<float>(center_y)) * cos_angle);

    // Use boundary condition to handle out-of-bounds accesses
    Func clamped = BoundaryConditions::repeat_edge(input);

    // Define the rotated image
    Func rotated("rotated");
    Expr mapped_x = forward_map(x, y)[0];
    Expr mapped_y = forward_map(x, y)[1];
    rotated(x, y, c) = clamped(clamp(cast<int>(mapped_x), 0, input.width() - 1),
                               clamp(cast<int>(mapped_y), 0, input.height() - 1),
                               c);

    // Apply the advanced scheduling techniques
    Var x_outer, y_outer, x_inner, y_inner, tile_index;
    rotated
        .tile(x, y, x_outer, y_outer, x_inner, y_inner, 64, 64)
        .fuse(x_outer, y_outer, tile_index)
        .parallel(tile_index)
        .vectorize(x_inner, 8)
        .unroll(y_inner);

    // Optimize memory access patterns
    rotated
        .align_bounds(x, 8)
        .align_bounds(y, 8);

    // Forward map computation should also be optimized
    forward_map.compute_at(rotated, y_outer).vectorize(x, 8);
    clamped.compute_at(rotated, y_outer);

    // Realize the output
    Buffer<uint8_t> output = rotated.realize({input.width(), input.height(), input.channels()});

    // Save the output
    save_image(output, "output.png");

    std::cout << "Image rotation complete." << std::endl;
    return 0;
}
