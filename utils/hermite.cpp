#include <opencv2/opencv.hpp>
#include <cmath>

using namespace cv;

// Function to perform Hermite interpolation
uchar hermiteInterpolation(const Mat& input, float x, float y) {
    int x0 = static_cast<int>(std::floor(x));
    int y0 = static_cast<int>(std::floor(y));
    float dx = x - x0;
    float dy = y - y0;

    // Cubic Hermite basis functions
    float s0 = 2 * pow(dx, 3) - 3 * pow(dx, 2) + 1;
    float s1 = -2 * pow(dx, 3) + 3 * pow(dx, 2);
    float s2 = pow(dx, 3) - 2 * pow(dx, 2) + dx;
    float s3 = pow(dx, 3) - pow(dx, 2);

    // Perform Hermite interpolation
    float interpolated_value = s0 * input.at<uchar>(y0, x0) +
                                s1 * input.at<uchar>(y0, x0 + 1) +
                                s2 * input.at<uchar>(y0 + 1, x0) +
                                s3 * input.at<uchar>(y0 + 1, x0 + 1);

    return static_cast<uchar>(interpolated_value);
}

int main() {
    // Open and load the input image
    Mat input = imread("../assets/Lenna_test_image.png", IMREAD_GRAYSCALE);

    // Define rotation angle in radians
    float angle = M_PI / 4; // 45 degrees

    // Define center of rotation
    float cx = input.cols / 2.0f;
    float cy = input.rows / 2.0f;

    // Create an empty image for the rotated output
    Mat output(input.rows, input.cols, CV_8U);

    // Perform rotation using Hermite interpolation
    for (int y = 0; y < output.rows; ++y) {
        for (int x = 0; x < output.cols; ++x) {
            // Compute original coordinates in the input image
            float x_orig = (x - cx) * cos(angle) + (y - cy) * sin(angle) + cx;
            float y_orig = -(x - cx) * sin(angle) + (y - cy) * cos(angle) + cy;

            // Perform Hermite interpolation to get pixel value
            output.at<uchar>(y, x) = hermiteInterpolation(input, x_orig, y_orig);
        }
    }

    // Display the rotated image
    imshow("Rotated Image", output);
    waitKey(0);
    return 0;
}
