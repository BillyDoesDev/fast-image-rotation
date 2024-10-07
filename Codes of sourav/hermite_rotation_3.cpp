#include <Halide.h>
#include <halide_image_io.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>

using namespace cv;
using namespace std;
using namespace std::chrono;
using namespace Halide;
using namespace Halide::Tools;

// Hermite function definition
Func hermite_function(int n, Expr x) {
    Func H("H" + std::to_string(n)); Var t("t");
    if (n == 0) {
        H(t) = 1.0f;
    } else if (n == 1) {
        H(t) = 2.0f * t;
    } else {
        Func Hn_1 = hermite_function(n - 1, x);
        Func Hn_2 = hermite_function(n - 2, x);
        H(t) = 2.0f * t * Hn_1(t) - 2.0f * (n - 1) * Hn_2(t);
    }
    H.compute_root();
    return H;
}

// Precompute Hermite functions
std::vector<Func> precompute_hermite_functions(int max_order, Expr size, std::string axis) {
    std::vector<Func> hermites;
    for (int n = 0; n < max_order; n++) {
        Func H = hermite_function(n, cast<float>(Var(axis)) / (size / 2.0f) - 1.0f);
        hermites.push_back(H);
    }
    return hermites;
}

// Perform Hermite expansion
Func hermite_expansion(Func image, int max_order, Expr rows, Expr cols, Expr channels) {
    Var n("n"), m("m"), c("c");
    Func coeffs("coeffs");
    coeffs(n, m, c) = 0.0f;

    RDom r(0, rows, 0, cols, 0, channels);
    for (int n = 0; n < max_order; n++) {
        for (int m = 0; m < max_order; m++) {
            Func Hn = hermite_function(n, cast<float>(r.x) / (rows / 2.0f) - 1.0f);
            Func Hm = hermite_function(m, cast<float>(r.y) / (cols / 2.0f) - 1.0f);
            coeffs(n, m, r.z) += image(r.x, r.y, r.z) * Hn(r.x) * Hm(r.y);
        }
    }
    return coeffs;
}

// Rotate the image
Func rotate_image(Func coeffs, float angle, int max_order, Expr channels) {
    Var n("n"), m("m"), c("c");
    Func rotated_coeffs("rotated_coeffs");

    // Calculate the rotation matrix elements
    float angle_radians = angle * static_cast<float>(M_PI) / 180.0f;
    Expr cos_angle = cos(angle_radians);
    Expr sin_angle = sin(angle_radians);

    // Initialize rotated_coeffs to zero
    rotated_coeffs(n, m, c) = 0.0f;

    RDom r(0, max_order, 0, max_order, 0, channels);
    Expr new_n = cast<int>(round(cos_angle * r.x - sin_angle * r.y));
    Expr new_m = cast<int>(round(sin_angle * r.x + cos_angle * r.y));
    rotated_coeffs(clamp(new_n, 0, max_order - 1), clamp(new_m, 0, max_order - 1), r.z) += coeffs(r.x, r.y, r.z);

    return rotated_coeffs;
}

// Reconstruct the image
Func reconstruct_image(Func coeffs, int max_order, Expr rows, Expr cols, Expr channels) {
    Var x("x"), y("y"), c("c");
    Func image_reconstructed("image_reconstructed");
    image_reconstructed(x, y, c) = 0.0f;

    auto hermites_x = precompute_hermite_functions(max_order, rows, "x");
    auto hermites_y = precompute_hermite_functions(max_order, cols, "y");

    for (int n = 0; n < max_order; n++) {
        for (int m = 0; m < max_order; m++) {
            image_reconstructed(x, y, c) += coeffs(n, m, c) * hermites_x[n](x) * hermites_y[m](y);
        }
    }

    image_reconstructed.compute_root(); // Ensuring the image is reconstructed once
    return image_reconstructed;
}

int main() {
    std::string inputImagePath = "../Test_Files/test_img_02.png";
    std::string outputImagePath = "../Test_Files/RH_test_img_02.png";

    Buffer<float> image = load_and_convert_image(inputImagePath);

    int max_order = 10; // Hermite expansion order
    float angle = 45.0f; // Rotate by 45 degrees
    int rows = image.height();
    int cols = image.width();
    int channels = image.channels();

    Func input_image("input_image");
    Var x("x"), y("y"), c("c");
    input_image(x, y, c) = image(x, y, c);

    std::cout << "Loading image from: " << inputImagePath << std::endl;
    std::cout << "Image loaded. Rows: " << rows << ", Cols: " << cols << ", Channels: " << channels << std::endl;

    // Perform Hermite expansion
    auto start_expansion = high_resolution_clock::now();
    Func coeffs = hermite_expansion(input_image, max_order, rows, cols, channels);
    Buffer<float> coeffs_realized = coeffs.realize({max_order, max_order, channels});
    auto end_expansion = high_resolution_clock::now();
    std::cout << "Hermite expansion completed in " << duration_cast<duration<double>>(end_expansion - start_expansion).count() << " seconds." << std::endl;

    // Rotate the image
    auto start_rotation = high_resolution_clock::now();
    Func rotated_coeffs = rotate_image(coeffs, angle, max_order, channels);
    Buffer<float> rotated_coeffs_realized = rotated_coeffs.realize({max_order, max_order, channels});
    auto end_rotation = high_resolution_clock::now();
    std::cout << "Image rotation completed in " << duration_cast<duration<double>>(end_rotation - start_rotation).count() << " seconds." << std::endl;

    // Reconstruct the image
    auto start_reconstruction = high_resolution_clock::now();
    Func rotated_image = reconstruct_image(rotated_coeffs, max_order, rows, cols, channels);
    Buffer<float> output_image = rotated_image.realize({rows, cols, channels});
    auto end_reconstruction = high_resolution_clock::now();
    std::cout << "Image reconstruction completed in " << duration_cast<duration<double>>(end_reconstruction - start_reconstruction).count() << " seconds." << std::endl;

    // Convert output image to uint8_t
    Buffer<uint8_t> final_output(rows, cols, channels);
    for (int c = 0; c < channels; c++) {
        for (int y = 0; y < cols; y++) {
            for (int x = 0; x < rows; x++) {
                final_output(x, y, c) = static_cast<uint8_t>(clamp(output_image(x, y, c), 0.0f, 1.0f) * 255.0f);
            }
        }
    }

    // Save rotated image
    save_image(final_output, outputImagePath);
    std::cout << "Image rotated by " << angle << " degrees and saved to " << outputImagePath << std::endl;

    return 0;
}
