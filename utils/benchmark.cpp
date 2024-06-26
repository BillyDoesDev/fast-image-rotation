#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char **argv) {

    if (argc < 3) {
        printf("Usage: program input_path angle\n");
        return -1;
    }

    Mat img = imread(argv[1]);
    if (img.empty()) {
        cerr << "Error loading image!\n";
        return -1;
    }

    int rows = img.rows;
    int cols = img.cols;

    double angle = stof(argv[2]);
    angle = angle * M_PI / 180.0; // Convert angle to radians

    double alpha = cos(angle);
    double beta = sin(angle);

    // Calculate the size of the rotated image
    int new_cols = static_cast<int>(abs(cols * alpha) + abs(rows * beta));
    int new_rows = static_cast<int>(abs(rows * alpha) + abs(cols * beta));

    Mat rotated(new_rows, new_cols, CV_8UC3, Scalar(0, 0, 0));

    double c_x = cols / 2.0;
    double c_y = rows / 2.0;
    double new_c_x = new_cols / 2.0;
    double new_c_y = new_rows / 2.0;

    // define matrix elements
    double m11 = alpha;
    double m12 = beta;
    double m13 = (1 - alpha) * new_c_x - beta * new_c_y + (c_x - new_c_x); // the last term in parenthesis represents the shift
    double m21 = -beta;
    double m22 = alpha;
    double m23 = beta * new_c_x + (1 - alpha) * new_c_y + (c_y - new_c_y); // the last term in parenthesis represents the shift

    for (int y = 0; y < new_rows; y++) {
        for (int x = 0; x < new_cols; x++) {
            int x_ = static_cast<int>(m11 * x + m12 * y + m13);
            int y_ = static_cast<int>(m21 * x + m22 * y + m23);

            if (x_ >= 0 && x_ < cols && y_ >= 0 && y_ < rows) {
                rotated.at<Vec3b>(y, x) = img.at<Vec3b>(y_, x_);
            }
        }
    }

    imshow("Rotated Image", rotated);
    waitKey();

    return 0;
}
