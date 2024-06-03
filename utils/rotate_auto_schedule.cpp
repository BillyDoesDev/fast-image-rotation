#include <stdio.h>
#include <cmath>

#include "Halide.h"
using namespace Halide;

class RotateImage : public Halide::Generator<RotateImage> {
public:
    Input<Buffer<float>> input{"input", 2};
    Input<float> angle{"angle"};
    Output<Buffer<float>> output{"output", 2};

    void generate() {
        // Define rotation matrix
        Expr c = cos(angle);
        Expr s = sin(angle);
        Expr tx = input.width() / 2;
        Expr ty = input.height() / 2;
        Expr x_old = cast<float>(x - tx);
        Expr y_old = cast<float>(y - ty);
        Expr x_new = c * x_old - s * y_old + tx;
        Expr y_new = s * x_old + c * y_old + ty;

        // Interpolation
        Expr xf = cast<float>(x_new);
        Expr yf = cast<float>(y_new);
        Expr xf0 = clamp(cast<int>(xf), 0, input.width() - 2);
        Expr yf0 = clamp(cast<int>(yf), 0, input.height() - 2);
        Expr xf1 = xf0 + 1;
        Expr yf1 = yf0 + 1;
        Expr dx = xf - xf0;
        Expr dy = yf - yf0;

        output(x, y) = lerp(
            lerp(input(xf0, yf0), input(xf1, yf0), dx),
            lerp(input(xf0, yf1), input(xf1, yf1), dx),
            dy
        );
    }

    void schedule() {
        if (using_autoscheduler()) {
            input.set_estimates({{0, 1024}, {0, 1024}});
            angle.set_estimate(0.5f);
            output.set_estimates({{0, 1024}, {0, 1024}});
        } else {
            // Schedule manually if not using autoscheduler
            // output.vectorize(x, 8).parallel(y);
            // we'll do nothing lol
        }
    }

private:
    Var x{"x"}, y{"y"};
};

HALIDE_REGISTER_GENERATOR(RotateImage, rotate_image_gen);
