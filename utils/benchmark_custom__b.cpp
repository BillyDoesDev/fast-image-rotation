// #include <cmath>
// #include <iostream>
// #include <opencv2/opencv.hpp>

// using namespace cv;
// using namespace std;

// int main(int argc, char **argv) {

//     if (argc < 4) {
//         printf("Usage: program input_path output_path angle\n");
//         return -1;
//     }

//     Mat img = imread(argv[1]);
//     if (img.empty()) {
//         cerr << "Error loading image!\n";
//         return -1;
//     }

//     int rows = img.rows;
//     int cols = img.cols;

//     double angle = stof(argv[3]);
//     angle = angle * M_PI / 180.0; // Convert angle to radians

//     double alpha = cos(angle);
//     double beta = sin(angle);

//     // Calculate the size of the rotated image
//     int new_cols = static_cast<int>(abs(cols * alpha) + abs(rows * beta));
//     int new_rows = static_cast<int>(abs(rows * alpha) + abs(cols * beta));

//     Mat rotated(new_rows, new_cols, CV_8UC3, Scalar(0, 0, 0));

//     // Reference:
//     // https://docs.opencv.org/3.4/da/d6e/tutorial_py_geometric_transformations.html
//     // https://docs.opencv.org/3.4/da/d54/group__imgproc__transform.html#gafbbc470ce83812914a70abfb604f4326
//     // https://docs.opencv.org/3.4/da/d54/group__imgproc__transform.html#ga0203d9ee5fcd28d40dbc4a1ea4451983

//     double c_x = cols / 2.0;
//     double c_y = rows / 2.0;
//     double new_c_x = new_cols / 2.0;
//     double new_c_y = new_rows / 2.0;

//     // define matrix elements
//     double m11 = alpha;
//     double m12 = beta;
//     double m13 = (1 - alpha) * new_c_x - beta * new_c_y + (c_x - new_c_x); // the last term in parenthesis represents the shift
//     double m21 = -beta;
//     double m22 = alpha;
//     double m23 = beta * new_c_x + (1 - alpha) * new_c_y + (c_y - new_c_y); // the last term in parenthesis represents the shift

//     for (int y = 0; y < new_rows; y++) {
//         for (int x = 0; x < new_cols; x++) {
//             int x_ = static_cast<int>(m11 * x + m12 * y + m13);
//             int y_ = static_cast<int>(m21 * x + m22 * y + m23);

//             if (x_ >= 0 && x_ < cols && y_ >= 0 && y_ < rows) {
//                 rotated.at<Vec3b>(y, x) = img.at<Vec3b>(y_, x_);
//             }
//         }
//     }

//     if (imwrite(argv[2], rotated)) {
//     } else {
//         cerr << "Error saving image" << endl;
//         return 1;
//     }

//     return 0;
// }

#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <omp.h>

using namespace cv;
using namespace std;

int main(int argc, char **argv) {

    if (argc < 3) {
        printf("Usage: program input_path output_path angle\n");
        return -1;
    }

    Mat img = imread(argv[1]);
    if (img.empty()) {
        cerr << "Error loading image!\n";
        return -1;
    }

    int rows = img.rows;
    int cols = img.cols;

    double angle = stod(argv[3]);
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

    // Define matrix elements
    double m11 = alpha;
    double m12 = beta;
    double m13 = (1 - alpha) * new_c_x - beta * new_c_y + c_x - new_c_x;
    double m21 = -beta;
    double m22 = alpha;
    double m23 = beta * new_c_x + (1 - alpha) * new_c_y + c_y - new_c_y;

    // Parallelize the loop using OpenMP
    #pragma omp parallel for collapse(2)
    for (int y = 0; y < new_rows; y++) {
        for (int x = 0; x < new_cols; x++) {
            int x_ = static_cast<int>(round(m11 * x + m12 * y + m13));
            int y_ = static_cast<int>(round(m21 * x + m22 * y + m23));

            if (x_ >= 0 && x_ < cols && y_ >= 0 && y_ < rows) {
                rotated.at<Vec3b>(y, x) = img.at<Vec3b>(y_, x_);
            }
        }
    }

    imwrite(argv[2], rotated);

    return 0;
}
