#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>

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

    // std::cout << "nrt=" << nrt << ", mrt=" << mrt << std::endl;

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
            for (int pixel = 0; pixel < n; ++pixel) {
                rot.at<cv::Vec3b>(b, a + x_offset) = img.at<cv::Vec3b>(line, pixel);
                rot.at<cv::Vec3b>(b + 1, a + x_offset) = img.at<cv::Vec3b>(line, pixel);

                a += 1;
                if (pixel > 0 && pixel % mod_factor == 0) {
                    b += 1;
                }
            }
        }
    }

    // Save the rotated image
    cv::imwrite("../outputs/opencv_dlr_rotated_image.png", rot);

    return 0;
}
