#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Halide-17.0.1-x86-64-linux/include/Halide.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    Mat img = imread("../assets/test.png", IMREAD_COLOR);
    if (!img.data) {
        printf("No image data \n");
        return -1;
    }

    int rows = img.rows;
    int cols = img.cols;

    float rot_angle = 30;
    float rot = rot_angle * M_PI / 180;

    // Calculate the size of the rotated image
    int new_rows = static_cast<int>(abs(rows * cos(rot)) + abs(cols * sin(rot)));
    int new_cols = static_cast<int>(abs(cols * cos(rot)) + abs(rows * sin(rot)));

    Mat rotated(new_rows, new_cols, CV_8UC3, Scalar(0, 0, 0));

    // Calculate the center of the original and the new image
    Point2f center_src(cols / 2.0, rows / 2.0);
    Point2f center_dst(new_cols / 2.0, new_rows / 2.0);

    // Define the rotation matrix
    Mat rotation_mat = (Mat_<float>(2, 2) << cosf(rot), -sinf(rot),
                        sinf(rot), cosf(rot));

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            // Translate pixel to origin
            Mat src_pos = (Mat_<float>(2, 1) << x - center_src.x, y - center_src.y);
            // Mat src_pos = (Mat_<float>(2, 1) << x, y);  <-- this is when you don't perform the center translation

            // Rotate the pixel
            Mat rotated_pos = rotation_mat * src_pos;

            // Translate pixel back
            float new_x = rotated_pos.at<float>(0, 0) + center_dst.x;
            float new_y = rotated_pos.at<float>(1, 0) + center_dst.y;
            // float new_x = rotated_pos.at<float>(0, 0);  <-- this is when you don't perform the center translation
            // float new_y = rotated_pos.at<float>(1, 0);

            int new_x_int = static_cast<int>(round(new_x));
            int new_y_int = static_cast<int>(round(new_y));

            // if (new_x_int >= 0 && new_x_int < new_cols && new_y_int >= 0 && new_y_int < new_rows) {
                rotated.at<Vec3b>(new_y_int, new_x_int) = img.at<Vec3b>(y, x);
            // }
        }
    }

    imshow("Original Image", img);
    imshow("Rotated Image", rotated);

    waitKey();

    return 0;
}
