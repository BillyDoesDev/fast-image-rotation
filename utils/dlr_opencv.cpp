#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>

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

    double angle = std::stoi(argv[3]);
    double alpha = angle * M_PI / 180.0;

    double sin_alpha = std::sin(alpha);
    double cos_alpha = std::cos(alpha);

    // use absoulute values here
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
    cv::imwrite(argv[2], rot);

    return 0;
}