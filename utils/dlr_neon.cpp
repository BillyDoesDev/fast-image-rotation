#include <arm_neon.h>
#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    // Load the image
    cv::Mat img = cv::imread("../assets/fish.png");
    if (img.empty()) {
        std::cerr << "Error loading image!" << std::endl;
        return -1;
    }

    int m = img.rows;
    int n = img.cols;

    double angle = 30;
    double alpha = angle * M_PI / 180.0;

    double sin_alpha = std::sin(alpha);
    double cos_alpha = std::cos(alpha);
    int nrt = static_cast<int>(std::ceil(m * sin_alpha)) + n;
    int mrt = static_cast<int>(std::ceil(m * cos_alpha + n * sin_alpha));

    cv::Mat rot = cv::Mat::zeros(mrt, nrt, img.type());

    int x_offset = static_cast<int>(std::ceil(m * sin_alpha));

    double rise = n * sin_alpha;
    double run = n * cos_alpha;
    int max_start_y = static_cast<int>(std::ceil(m * cos_alpha));
    auto fs = [&](int y) { return y / std::tan(alpha + M_PI / 2); };

    double y_incr = std::floor(n / rise);
    double x_incr = std::floor(n / run);

    if (y_incr > x_incr) {
        int mod_factor = static_cast<int>(y_incr / x_incr);
        for (int line = 0; line < max_start_y; ++line) {
            int a = static_cast<int>(std::ceil(fs(line)));
            int b = line;

            for (int pixel = 0; pixel < n; pixel += 4) { // Process 4 pixels at a time
                // Load 4 pixels
                uint8x16x3_t src_pixels = vld3q_u8(&img.at<cv::Vec3b>(line, pixel)[0]);

                // Store 4 pixels to destination with NEON intrinsics
                int base_idx = (b * rot.cols + (a + x_offset)) * 3;
                int base_idx_next_row = ((b + 1) * rot.cols + (a + x_offset)) * 3;
                vst3q_u8(&rot.data[base_idx], src_pixels);
                vst3q_u8(&rot.data[base_idx_next_row], src_pixels);

                a += 4; // Increment by 4 for the next batch of pixels
                if (pixel > 0 && pixel % mod_factor == 0) {
                    b += 2;
                }
            }
        }
    }

    // Save the rotated image
    cv::imwrite("../outputs/neon_dlr_rotated_image.png", rot);

    return 0;
}
