#include <cmath>
#include <functional>
#include <iostream>
#include <opencv2/opencv.hpp>

using cv::Mat;
using std::floor, std::ceil, std::abs, std::sin, std::cos, std::tan, std::function;

void rotate(const Mat &img, Mat &rot, int mrt, int nrt, int x_offset, int y_offset, double delta_x, double delta_y, double delta_i,
            function<double(int)> fs, int outer_limit, int last_px, bool horizontal,
            bool negate_coords, bool flip_range, bool use_abs_mapping = false, bool flip_outer = false) {

    double src_i = 0;
    int shift = negate_coords ? -1 : 1;
    int start, stop, increment;
    int a, b, a_, b_;

    double x_, y_;
    int x, y, i_;
    int px = 0;
    bool check;

    if (flip_range) {
        start = last_px;
        stop = -1;
        increment = -1;
    } else {
        start = 0;
        stop = last_px;
        increment = 1;
    }

    

    #pragma omp parallel for
    for (int i = 0; i < outer_limit; i++) {
        x_ = fs(i);
        y_ = i;

        i_ = floor(src_i);
        for (px = start; flip_range ? px > stop : px < stop; px += increment) {
            x = floor(x_);
            y = floor(y_);

            if (horizontal) {
                if (use_abs_mapping) {
                    a = abs(i_);
                    b = abs(px);
                } else {
                    a = i_;
                    b = px;
                }
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

            // printf("a_, b_ = %d, %d\n", a_, b_);
            // printf("attempting to map (%d, %d) to (%d, %d)\n", a_, b_, a, b);

            if (a_ >= 0 && a_ < mrt && b_ >= 0 && b_ < nrt)
                rot.at<cv::Vec3b>(a_, b_) = img.at<cv::Vec3b>(a, b);
            if (a_ + 1 >= 0 && a_ + shift < mrt && b_ + 1 >= 0 && b_ + 1 < nrt)
                rot.at<cv::Vec3b>(a_ + horizontal, b_ + !horizontal) = img.at<cv::Vec3b>(a, b);

            x_ += delta_x;
            y_ += delta_y;
        }
        src_i += delta_i;
    }
}

int main(int argc, char const *argv[]) {
    Mat img = cv::imread("../assets/fish.png");
    int m = img.rows;
    int n = img.cols;

    double angle = 350;
    double alpha = (M_PI * angle) / 180.0;

    double sin_alpha = abs(sin(alpha));
    double cos_alpha = abs(cos(alpha));
    double tan_alpha = tan(alpha);
    double tan_alpha_ = tan(alpha + M_PI / 2.0);

    int mrt = ceil(m * cos_alpha + n * sin_alpha);
    int nrt = ceil(m * sin_alpha + n * cos_alpha);

    Mat rot = Mat::zeros(mrt, nrt, img.type());
    // printf("m, n = %d, %d | mrt, nrt = %d, %d\n", m, n, mrt, nrt);

    function<double(int)> fs; // returns double, takes in int
    double delta_x, delta_y, delta_i;
    int x_offset, y_offset, outer_limit, last_px;

    // zone 1 and 5
    if ((0 <= angle && angle <= 45) || (180 < angle && angle <= 225)) {
        printf("zone 1 or 5\n");
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
        printf("zone 2 or 6\n");
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
        printf("zone 3 or 7\n");
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
        printf("zone 4 or 8\n");
        x_offset = ceil(n * cos_alpha);
        y_offset = 0;
        fs = [tan_alpha_](int y) { return y / tan_alpha_; };
        delta_x = -cos_alpha;
        delta_y = sin_alpha;
        delta_i = 1 / delta_x;

        outer_limit = ceil(m * cos_alpha);
        last_px = n;
        if (135 < angle && angle <= 180)
            rotate(img, rot, mrt, nrt, x_offset, y_offset, delta_x, delta_y, delta_i, fs, outer_limit, last_px, true, false, false, true); // zone 4 sucks ass dude
        else
            rotate(img, rot, mrt, nrt, x_offset, y_offset, delta_x, delta_y, delta_i, fs, outer_limit, last_px, true, false, true, true); // zone 4 sucks ass dude
    }

    cv::imwrite("../lol.png", rot);

    return 0;
}
