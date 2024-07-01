#include <cmath>
#include <functional>
#include <iostream>
#include <omp.h>
#include <opencv2/opencv.hpp>

using cv::Mat;
using std::floor, std::ceil, std::abs, std::sin, std::cos, std::tan, std::function;

void rotate(const Mat &img, Mat &rot, int mrt, int nrt, int x_offset, int y_offset, double delta_x, double delta_y, double delta_i,
            function<double(int)> fs, int outer_limit, int last_px,
            bool horizontal, bool negate_coords, bool flip_range, bool z48 = false) {

    int a, b, a_, b_;

    double x_, y_;
    int x, y, i_;

    if (z48) {
#pragma omp parallel for
        for (int i = outer_limit; i > -1; i--) {
            int index = i - outer_limit;
            double src_i = index * delta_i;

            x_ = fs(i);
            y_ = i;

            i_ = floor(src_i);
            for (int px = 0; px < last_px; px++) {
                x = floor(x_);
                y = floor(y_);

                a = abs(i_);
                b = abs(px);

                if (negate_coords) {
                    a_ = mrt - y - y_offset;
                    b_ = nrt - x - x_offset;
                } else {
                    a_ = y + y_offset;
                    b_ = x + x_offset;
                }

                if (a_ >= 0 && a_ < mrt && b_ >= 0 && b_ < nrt) {
                    // std::cout << "attempting to map (" << a_ << ", " << b_ << ") to (" << a << ", " << b << ")\n";
                    rot.at<cv::Vec3b>(a_, b_) = img.at<cv::Vec3b>(a, b);
                }
                if (a_ + 1 >= 0 && a_ + 1 < mrt && b_ >= 0 && b_ < nrt)
                    rot.at<cv::Vec3b>(a_ + 1, b_) = img.at<cv::Vec3b>(a, b);

                x_ += delta_x;
                y_ += delta_y;
            }
        }
        return;
    }

#pragma omp parallel for
    for (int i = 0; i < outer_limit; i++) {
        double src_i = i * delta_i;

        x_ = fs(i);
        y_ = i;

        i_ = floor(src_i);

        if (flip_range) {
            for (int px = last_px; px > -1; px--) {
                x = floor(x_);
                y = floor(y_);

                if (horizontal) {
                    a = i_;
                    b = px;
                } else {
                    a = px;
                    b = i_;
                }

                // when do we do this even?
                // ...when we are fated to deal with negative indices
                if (negate_coords) {
                    a_ = mrt - y - y_offset;
                    b_ = nrt - x - x_offset;
                } else {
                    a_ = y + y_offset;
                    b_ = x + x_offset;
                }
                if (a < 0)
                    a += 2880;
                if (b < 0)
                    b += 6041;
                // a_ = y + y_offset;
                // b_ = x + x_offset;
                // if (a_ < 0)
                //     a_ = mrt - y - y_offset;
                // if (b_ < 0)
                //     b_ = nrt - x - x_offset;

                    // m, n = 2880, 6041

                if ((a_ >= 0 && a_ < mrt) && (b_ >= 0 && b_ < nrt) &&
                (a >= 0 && a < 2880) && (b >= 0 && b < 6041)) {
                    // std::cout << "a: " << a << ", b: " << b << ", <- a_: " << a_ << ", b_: " << b_ << "\n";
                    rot.at<cv::Vec3b>(a_, b_) = img.at<cv::Vec3b>(a, b);
                }
                // if (a_ + horizontal >= 0 && a_ + horizontal < mrt && b_ + !horizontal >= 0 && b_ + !horizontal < nrt)
                //     rot.at<cv::Vec3b>(a_ + horizontal, b_ + !horizontal) = img.at<cv::Vec3b>(a, b);

                x_ += delta_x;
                y_ += delta_y;
            }
        } else {
            for (int px = 0; px < last_px; px++) {
                x = floor(x_);
                y = floor(y_);

                if (horizontal) {
                    a = i_;
                    b = px;
                } else {
                    a = px;
                    b = i_;
                }

                if (negate_coords) {
                    a_ = mrt - y - y_offset;
                    b_ = nrt - x - x_offset;
                } else {
                    a_ = y + y_offset;
                    b_ = x + x_offset;
                }

                if (a_ >= 0 && a_ < mrt && b_ >= 0 && b_ < nrt)
                    rot.at<cv::Vec3b>(a_, b_) = img.at<cv::Vec3b>(a, b);
                if (a_ + horizontal >= 0 && a_ + horizontal < mrt && b_ + !horizontal >= 0 && b_ + !horizontal < nrt)
                    rot.at<cv::Vec3b>(a_ + horizontal, b_ + !horizontal) = img.at<cv::Vec3b>(a, b);

                x_ += delta_x;
                y_ += delta_y;
            }
        }
    }
}

int main(int argc, char const *argv[]) {
    printf("argc: %d\n", argc);
    // if (argc < 4) {
    //     printf("Usage: program input_path output_path angle\n");
    //     return -1;
    // }

    // Mat img = cv::imread(argv[1]);
    Mat img = cv::imread("../assets/fish_2880.png");
    int m = img.rows;
    int n = img.cols;

    // double angle = std::stod(argv[3]);
    double angle = 226;
    double alpha = (M_PI * angle) / 180.0;

    double sin_alpha = abs(sin(alpha));
    double cos_alpha = abs(cos(alpha));
    double tan_alpha = tan(alpha);
    double tan_alpha_ = tan(alpha + M_PI / 2.0);

    int mrt = ceil(m * cos_alpha + n * sin_alpha);
    int nrt = ceil(m * sin_alpha + n * cos_alpha);

    Mat rot = Mat::zeros(mrt, nrt, img.type());
    printf("m, n = %d, %d | mrt, nrt = %d, %d\n", m, n, mrt, nrt);

    function<double(int)> fs; // returns double, takes in int
    double delta_x, delta_y, delta_i;
    int x_offset, y_offset, outer_limit, last_px;

    // zone 1 and 5
    if ((0 <= angle && angle <= 45) || (180 < angle && angle <= 225)) {
        x_offset = ceil(m * sin_alpha);
        y_offset = 0;
        fs = [tan_alpha_](int y) { return y / tan_alpha_; };
        delta_x = cos_alpha;
        delta_y = sin_alpha;
        delta_i = 1 / delta_x;

        outer_limit = ceil(m * cos_alpha);
        last_px = n;
        rotate(img, rot, mrt, nrt, x_offset, y_offset, delta_x, delta_y, delta_i, fs, outer_limit, last_px, true, !(0 <= angle && angle <= 45), false);
    }

    // zone 2 and 6
    else if ((45 < angle && angle <= 90) || (225 < angle && angle <= 270)) {
        x_offset = ceil(m * sin_alpha);
        y_offset = 0;
        fs = [tan_alpha](int y) { return y / tan_alpha; };
        delta_x = -sin_alpha;
        delta_y = cos_alpha;
        delta_i = ((45 < angle && angle <= 90) ? -1 : 1) / delta_x;

        outer_limit = ceil(n * sin_alpha);
        last_px = m;
        rotate(img, rot, mrt, nrt, x_offset, y_offset, delta_x, delta_y, delta_i, fs, outer_limit, last_px, false, false, !(45 < angle && angle <= 90));
    }

    // zone 3 and 7
    else if ((90 < angle && angle <= 135) || (270 < angle && angle <= 315)) {
        x_offset = nrt;
        y_offset = ceil(m * cos_alpha);
        fs = [tan_alpha](int y) { return y / tan_alpha; };
        delta_x = -sin_alpha;
        delta_y = -cos_alpha;
        delta_i = ((90 < angle && angle <= 135) ? -1 : 1) / delta_x;

        outer_limit = ceil(n * sin_alpha);
        last_px = m;
        rotate(img, rot, mrt, nrt, x_offset, y_offset, delta_x, delta_y, delta_i, fs, outer_limit, last_px, false, false, !(90 < angle && angle <= 135));
    }

    // zone 4 and 8
    else if ((135 < angle && angle <= 180) || (315 < angle && angle <= 360)) {
        // std::cout << "z48\n";
        x_offset = ceil(n * cos_alpha);
        y_offset = 0;
        fs = [tan_alpha_](int y) { return y / tan_alpha_; };
        delta_x = -cos_alpha;
        delta_y = sin_alpha;
        delta_i = 1 / delta_x;

        outer_limit = ceil(m * cos_alpha);
        last_px = n;
        rotate(img, rot, mrt, nrt, x_offset, y_offset, delta_x, delta_y, delta_i, fs, outer_limit, last_px, true, !(135 < angle && angle <= 180), false, true);
    }

    // cv::imwrite(argv[2], rot);
    cv::imwrite("../lol.png", rot);

    return 0;
}
