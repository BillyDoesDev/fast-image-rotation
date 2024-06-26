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
    angle = angle * M_PI / 180;

    // https://docs.opencv.org/3.4/da/d6e/tutorial_py_geometric_transformations.html
    // https://docs.opencv.org/3.4/da/d54/group__imgproc__transform.html#gafbbc470ce83812914a70abfb604f4326

    // get rotation matrix 2d
    // leaving `scale` = 1
    double alpha = cos(angle);
    double beta = sin(angle);

    // Calculate the size of the rotated image
    int new_rows = static_cast<int>(abs(rows * alpha) + abs(cols * beta));
    int new_cols = static_cast<int>(abs(cols * alpha) + abs(rows * beta));

    Mat rotated(new_rows, new_cols, CV_8UC3, Scalar(0, 0, 0));

    // double c_x = (cols - 1) / 2.0;
    // double c_y = (rows - 1) / 2.0;
    double c_x = (new_cols - 1) / 2.0;
    double c_y = (new_rows - 1) / 2.0;

    // define matrix elements
    double m11 = alpha;
    double m12 = beta;
    double m13 = (1 - alpha) * c_x - beta * c_y;
    double m21 = -beta;
    double m22 = alpha;
    double m23 = beta * c_x + (1 - alpha) * c_y;

    for (int y = 0; y < new_rows; y++) {
        for (int x = 0; x < new_cols; x++) {
            int y_ = static_cast<int>(round(m11 * x + m12 * y + m13));
            int x_ = static_cast<int>(round(m21 * x + m22 * y + m23));
            rotated.at<Vec3b>(y, x) = img.at<Vec3b>(y_, x_);
        }
    }

    imshow("lol.png", rotated);
    waitKey();

    return 0;
}