#include <cmath>

#include "Halide.h"
#include "halide_image_io.h"

using namespace Halide;
using namespace Halide::Tools;

int main(int argc, char** argv) {
    // Read the input image using Halide
    Buffer<uint8_t> img = load_image("../assets/Lenna_test_image.png");

    int rows = img.height();
    int cols = img.width();

    float rot_angle = 30.0f;
    float rot = rot_angle * M_PI / 180.0f;

    // Calculate the size of the rotated image
    int new_rows = static_cast<int>(std::abs(rows * std::cos(rot)) + std::abs(cols * std::sin(rot)));
    int new_cols = static_cast<int>(std::abs(cols * std::cos(rot)) + std::abs(rows * std::sin(rot)));

    // Calculate the center of the original and the new image
    float center_src_x = cols / 2.0f;
    float center_src_y = rows / 2.0f;
    float center_dst_x = new_cols / 2.0f;
    float center_dst_y = new_rows / 2.0f;

    // Define the rotation matrix elements
    float cos_rot = std::cos(rot);
    float sin_rot = std::sin(rot);

    // Define Halide variables
    Var x("x"), y("y"), c("c");

    // Define the rotation transformation
    Expr src_x = (x - center_dst_x) * cos_rot + (y - center_dst_y) * sin_rot + center_src_x;
    Expr src_y = -(x - center_dst_x) * sin_rot + (y - center_dst_y) * cos_rot + center_src_y;

    // Use boundary condition to handle out-of-bounds accesses
    Func clamped = BoundaryConditions::repeat_edge(img);

    // Define Halide function for the rotated image
    Func rotated("rotated");
    rotated(x, y, c) = clamped(clamp(cast<int>(src_x), 0, img.width() - 1),
                               clamp(cast<int>(src_y), 0, img.height() - 1),
                               c);

    // Schedule the function
    rotated.parallel(y).vectorize(x, 8);

    // Realize the rotated image
    Buffer<uint8_t> result = rotated.realize({new_cols, new_rows, img.channels()});

    // rotated.print_loop_nest();

    save_image(result, "../outputs/halide_output.png");

    return 0;
}
