#include <cmath>
#include <iostream>
#include <omp.h>
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: program input_path output_path angle\n");
        return -1;
    }

    // Load the image
    cv::Mat img = cv::imread(argv[1]);
    if (img.empty()) {
        std::cerr << "Error loading image!" << std::endl;
        return -1;
    }

    int m = img.rows;
    int n = img.cols;

    double angle = std::stod(argv[3]); // Use std::stod instead of std::stoi
    double alpha = (angle * M_PI) / 180.0;

    double sin_alpha = std::sin(alpha);
    double cos_alpha = std::cos(alpha);

    // Use absolute values here
    int nrt = std::ceil(std::abs(n * cos_alpha) + std::abs(m * sin_alpha));
    int mrt = std::ceil(std::abs(m * cos_alpha) + std::abs(n * sin_alpha));

    cv::Mat rot = cv::Mat::zeros(mrt, nrt, img.type());

    int x_offset = static_cast<int>(std::ceil(m * sin_alpha));

    double delta_x = cos_alpha;
    double delta_y = sin_alpha;
    int end_pt = static_cast<int>(std::ceil(m * cos_alpha));
    double K = std::tan(alpha + M_PI / 2);

    // Parallelize the outer loop using OpenMP
    int i__;
    double delta_i = 1 / delta_x;
#pragma omp parallel for
    for (int i = 0; i < end_pt; ++i) {
        double i_ = i * delta_i; // Each thread initializes its own i_
        double x_ = i / K;
        double y_ = i;

        i__ = floor(i_);
        for (int px = 0; px < n; ++px) {
            int x = std::floor(x_);
            int y = std::floor(y_);

            // Ensure that access is within bounds
            if (y >= 0 && y < mrt && x + x_offset >= 0 && x + x_offset < nrt) {
                rot.at<cv::Vec3b>(y, x + x_offset) = img.at<cv::Vec3b>(i__, px);
            }
            if (y + 1 >= 0 && y + 1 < mrt && x + x_offset >= 0 && x + x_offset < nrt) {
                rot.at<cv::Vec3b>(y + 1, x + x_offset) = img.at<cv::Vec3b>(i__, px);
            }

            x_ += delta_x;
            y_ += delta_y;
        }
    }

    // Save the rotated image
    cv::imwrite(argv[2], rot);

    return 0;
}
