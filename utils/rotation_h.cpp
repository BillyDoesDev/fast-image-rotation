#include "Halide.h"
#include "halide_image_io.h"
#include <iostream>
#include <cmath>
#include <vector>

using namespace Halide;
using namespace Halide::Tools;

// Function to define Hermite polynomials
Func hermite_polynomial(int n) {
    Var x;
    Func H_n;
    if (n == 0) {
        H_n(x) = 1;
    } else if (n == 1) {
        H_n(x) = 2 * x;
    } else {
        Func H_nm1 = hermite_polynomial(n - 1);
        Func H_nm2 = hermite_polynomial(n - 2);
        H_n(x) = 2 * x * H_nm1(x) - 2 * (n - 1) * H_nm2(x);
    }
    return H_n;
}

// Function to compute Hermite coefficients
Func compute_hermite_coefficients(Buffer<float> input, int order) {
    Var x, y;
    Func coeff("coeff");
    RDom r(0, input.width(), 0, input.height());

    std::vector<Func> H(order + 1);
    for (int n = 0; n <= order; n++) {
        H[n] = hermite_polynomial(n);
    }

    for (int m = 0; m <= order; m++) {
        for (int n = 0; n <= order; n++) {
            Expr sum_val = 0.0f;
            for (int i = 0; i < input.width(); i++) {
                for (int j = 0; j < input.height(); j++) {
                    sum_val += input(i, j) * H[m](i) * H[n](j);
                }
            }
            coeff(m, n) = sum_val;
        }
    }
    return coeff;
}

// Function to rotate Hermite coefficients
Func rotate_hermite_coefficients(Func coeff, int order, float angle) {
    Var m, n;
    Func rotated_coeff("rotated_coeff");

    float cos_angle = std::cos(angle);
    float sin_angle = std::sin(angle);

    rotated_coeff(m, n) = 0.0f;
    for (int k = 0; k <= order; k++) {
        for (int l = 0; l <= order; l++) {
            rotated_coeff(m, n) += coeff(k, l) *
                (cos_angle * k - sin_angle * l) *
                (sin_angle * k + cos_angle * l);
        }
    }

    return rotated_coeff;
}

// Function to reconstruct image from rotated Hermite coefficients
Func reconstruct_image(Func rotated_coeff, int width, int height, int order) {
    Var x, y;
    Func output("output");

    std::vector<Func> H(order + 1);
    for (int n = 0; n <= order; n++) {
        H[n] = hermite_polynomial(n);
    }

    output(x, y) = 0.0f;
    for (int m = 0; m <= order; m++) {
        for (int n = 0; n <= order; n++) {
            output(x, y) += rotated_coeff(m, n) * H[m](x) * H[n](y);
        }
    }

    return output;
}

int main(int argc, char **argv) {
    try {
        // Load input image
        std::string inputImagePath = "../assets/fish.png";
        std::string outputImagePath = "../outputs/RH_test_05.png";
        
        Buffer<uint8_t> input = load_image(inputImagePath);

        int width = input.width();
        int height = input.height();
        int order = 10;  // Order of Hermite expansion
        float angle = M_PI / 6;  // 30 degrees

        // Convert input image to float
        Buffer<float> input_float(width, height);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                input_float(x, y) = input(x, y) / 255.0f;
            }
        }

        // Compute Hermite coefficients
        Func func_coeff = compute_hermite_coefficients(input_float, order);

        // Rotate Hermite coefficients
        Func rotated_coeff = rotate_hermite_coefficients(func_coeff, order, angle);

        // Reconstruct the image from rotated Hermite coefficients
        Func output = reconstruct_image(rotated_coeff, width, height, order);

        // Realize the output image
        Buffer<float> output_image = output.realize({width, height});

        // Convert output image to uint8 for saving
        Buffer<uint8_t> output_uint8(width, height);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                output_uint8(x, y) = (uint8_t)(output_image(x, y) * 255.0f);
            }
        }

        // Save the output image
        save_image(output_uint8, outputImagePath);

        std::cout << "Rotation complete. Rotated image saved as " << outputImagePath << std::endl;
    } catch (const Halide::Error &e) {
        std::cerr << "Halide Error: " << e.what() << std::endl;
        return -1;
    } catch (const std::exception &e) {
        std::cerr << "Standard Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}