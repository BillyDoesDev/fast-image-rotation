#include <cmath>
#include <iostream>
#include <omp.h>
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[]) {
    // if (argc < 4) {
    //     printf("Usage: program input_path output_path angle\n");
    //     return -1;
    // }

    cv::Mat img = cv::imread("../assets/fish.png"); // FUCK YOU if you can't even load a FUCKING IMAGE properly

    int m = img.rows;
    int n = img.cols;

    double angle = 270;
    double alpha = (angle * M_PI) / 180.0;

    double sin_alpha = std::abs(std::sin(alpha));
    double cos_alpha = std::abs(std::cos(alpha));
    double tan_alpha = std::tan(alpha);
    double tan_alpha_ = std::tan(alpha + M_PI / 2.0);

    int nrt = std::ceil(n * cos_alpha + m * sin_alpha);
    int mrt = std::ceil(m * cos_alpha + n * sin_alpha);

    cv::Mat rot = cv::Mat::zeros(mrt, nrt, img.type());

    // zone 1 or 5
    if ((0 <= angle && angle <= 45) || (180 < angle && angle <= 225)) {
        int x_offset = std::ceil(m * sin_alpha);

        double delta_x = cos_alpha;
        double delta_y = sin_alpha;
        int end_pt = std::ceil(m * cos_alpha);

        int i__;
        double delta_i = 1 / delta_x;

#pragma omp parallel for
        for (int i = 0; i < end_pt; ++i) {
            double i_ = i * delta_i; // Each thread initializes its own i_

            // (x_, y_) = (fs(i), i)
            double x_ = i / tan_alpha_;
            double y_ = i;

            i__ = floor(i_);
            for (int px = 0; px < n; ++px) {
                int x = std::floor(x_);
                int y = std::floor(y_);

                // changes based on whether horizontal or not
                int a = i__;
                int b = px;

                // changes based on negate_coords
                int a_, b_;
                if (0 <= angle && angle <= 45) {
                    a_ = y;
                    b_ = x + x_offset;
                } else {
                    a_ = mrt - y;
                    b_ = nrt - x - x_offset;
                }

                if (a_ >= 0 && a_ < mrt && b_ >= 0 && b_ < nrt)
                    rot.at<cv::Vec3b>(a_, b_) = img.at<cv::Vec3b>(a, b);
                if (a_ + 1 >= 0 && a_ + 1 < mrt && b_ >= 0 && b_ < nrt)
                    rot.at<cv::Vec3b>(a_ + 1, b_) = img.at<cv::Vec3b>(a, b);

                x_ += delta_x;
                y_ += delta_y;
            }
        }
    }

    // zone 2 and 6
    else if ((45 < angle && angle <= 90) || (225 < angle && angle <= 270)) {
        int x_offset = std::ceil(m * sin_alpha);

        double delta_x = -sin_alpha;
        double delta_y = cos_alpha;
        int end_pt = std::ceil(n * sin_alpha);

        int i__;
        double delta_i = ((45 < angle && angle <= 90) ? -1 : 1) / delta_x;

#pragma omp parallel for
        for (int i = 0; i < end_pt; ++i) {
            double i_ = i * delta_i; // Each thread initializes its own i_

            // (x_, y_) = (fs(i), i)
            double x_ = i / tan_alpha;
            double y_ = i;

            i__ = floor(i_);

            // changes based on flip_range
            if (225 < angle && angle <= 270) {
                for (int px = m; px > -1; px--) {
                    int x = std::floor(x_);
                    int y = std::floor(y_);

                    // changes based on whether horizontal or not
                    int a = px;
                    int b = i__;

                    // changes based on negate_coords
                    int a_, b_;
                    a_ = y;
                    b_ = x + x_offset;

                    if (a_ >= 0 && a_ < mrt && b_ >= 0 && b_ < nrt)
                        rot.at<cv::Vec3b>(a_, b_) = img.at<cv::Vec3b>(a, b);
                    if (a_ + 1 >= 0 && a_ + 1 < mrt && b_ >= 0 && b_ < nrt)
                        rot.at<cv::Vec3b>(a_ + 1, b_) = img.at<cv::Vec3b>(a, b);

                    x_ += delta_x;
                    y_ += delta_y;
                }
            } else {
                for (int px = 0; px < m; ++px) {
                    int x = std::floor(x_);
                    int y = std::floor(y_);

                    // changes based on whether horizontal or not
                    int a = px;
                    int b = i__;

                    // changes based on negate_coords
                    int a_, b_;
                    a_ = y;
                    b_ = x + x_offset;

                    if (a_ >= 0 && a_ < mrt && b_ >= 0 && b_ < nrt)
                        rot.at<cv::Vec3b>(a_, b_) = img.at<cv::Vec3b>(a, b);
                    if (a_ + 1 >= 0 && a_ + 1 < mrt && b_ >= 0 && b_ < nrt)
                        rot.at<cv::Vec3b>(a_ + 1, b_) = img.at<cv::Vec3b>(a, b);

                    x_ += delta_x;
                    y_ += delta_y;
                }
            }
        }
    }

    cv::imwrite("../lol.png", rot);

    return 0;
}
