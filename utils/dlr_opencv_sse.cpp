#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <xmmintrin.h> // SSE intrinsics

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

            // Pointers to the image data for faster access
            const uchar *src_ptr = img.ptr<uchar>(line);
            uchar *dst_ptr = rot.ptr<uchar>(b);
            uchar *dst_ptr_next = rot.ptr<uchar>(b + 1);

            for (int pixel = 0; pixel < n * 3; pixel += 12) { // Process 4 pixels (12 bytes) at a time
                // Load 4 pixels at a time using SSE
                __m128i pixels = _mm_loadu_si128(reinterpret_cast<const __m128i *>(&src_ptr[pixel]));

                // Store these pixels in the destination image
                _mm_storeu_si128(reinterpret_cast<__m128i *>(&dst_ptr[(a * 3) + x_offset * 3]), pixels);
                _mm_storeu_si128(reinterpret_cast<__m128i *>(&dst_ptr_next[(a * 3) + x_offset * 3]), pixels);

                a += 4; // Since we process 4 pixels at a time
                if (pixel > 0 && pixel % mod_factor == 0) {
                    b += 2;
                    dst_ptr = rot.ptr<uchar>(b);
                    dst_ptr_next = rot.ptr<uchar>(b + 1);
                }
            }
        }
    }

    // Save the rotated image
    cv::imwrite("../outputs/opencv_dlr_sse_rotated_image.png", rot);

    return 0;
}
