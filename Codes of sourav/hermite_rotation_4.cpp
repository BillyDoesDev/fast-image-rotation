#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace cv;
using namespace std;
using namespace std::chrono;

// Hermite polynomial calculation (recurrence relation)
double hermite(int n, double x) {
    if (n == 0) return 1.0;
    if (n == 1) return 2.0 * x;
    return 2.0 * x * hermite(n - 1, x) - 2.0 * (n - 1) * hermite(n - 2, x);
}

// Compute Hermite coefficients for a given image channel
std::vector<std::vector<double>> hermiteExpansion(const cv::Mat& imageChannel, int order) {
    int rows = imageChannel.rows;
    int cols = imageChannel.cols;
    std::vector<std::vector<double>> coeffs(order, std::vector<double>(order, 0.0));

    for (int m = 0; m < order; ++m) {
        for (int n = 0; n < order; ++n) {
            double sum = 0.0;
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    double x = (2.0 * i / rows) - 1.0;
                    double y = (2.0 * j / cols) - 1.0;
                    sum += static_cast<double>(imageChannel.at<uchar>(i, j)) * hermite(m, x) * hermite(n, y);
                }
            }
            coeffs[m][n] = sum / (rows * cols); // Normalizing by the number of pixels
        }
    }
    return coeffs;
}

// Rotate a single image channel using Hermite interpolation
cv::Mat rotateChannelUsingHermite(const cv::Mat& imageChannel, double angle, int order) {
    int rows = imageChannel.rows;
    int cols = imageChannel.cols;
    cv::Mat rotatedChannel(rows, cols, CV_64F, cv::Scalar(0.0));

    double cosTheta = std::cos(angle);
    double sinTheta = std::sin(angle);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            double x = (2.0 * i / rows) - 1.0;
            double y = (2.0 * j / cols) - 1.0;
            double newX = cosTheta * x - sinTheta * y;
            double newY = sinTheta * x + cosTheta * y;

            // Convert newX, newY back to image pixel coordinates
            int newI = static_cast<int>((newX + 1.0) * rows / 2.0);
            int newJ = static_cast<int>((newY + 1.0) * cols / 2.0);

            if (newI >= 0 && newI < rows && newJ >= 0 && newJ < cols) {
                rotatedChannel.at<double>(newI, newJ) = static_cast<double>(imageChannel.at<uchar>(i, j));
            }
        }
    }

    // Normalize the channel to the range [0, 255]
    double minVal, maxVal;
    cv::minMaxLoc(rotatedChannel, &minVal, &maxVal);
    rotatedChannel.convertTo(rotatedChannel, CV_8UC1, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));

    return rotatedChannel;
}

// Rotate the entire image using Hermite interpolation for each channel
cv::Mat rotateImageUsingHermite(const cv::Mat& image, double angle, int order) {
    std::vector<cv::Mat> channels(3);
    cv::split(image, channels);  // Split the image into B, G, R channels

    // Rotate each channel separately
    for (int i = 0; i < 3; ++i) {
        channels[i] = rotateChannelUsingHermite(channels[i], angle, order);
    }

    cv::Mat rotatedImage;
    cv::merge(channels, rotatedImage);  // Merge the rotated channels back

    return rotatedImage;
}

int main() {
    // Load the color image
    std::string inputImagePath = "../Test_Files/test_img_02.png";
    std::string outputImagePath = "../Rotated Images/RH_test_img_02.png";
    cv::Mat image = cv::imread(inputImagePath, cv::IMREAD_COLOR);
    
    if (image.empty()) {
        std::cerr << "Error: Image cannot be loaded." << std::endl;
        return -1;
    }

    int order = 10;  // Example order of Hermite expansion
    double angle = CV_PI / 4;  // 45 degrees

    // Rotate the color image using Hermite interpolation
    cv::Mat rotatedImage = rotateImageUsingHermite(image, angle, order);

    // Save the rotated image
    if (imwrite(outputImagePath, rotatedImage)) {
        cout << "Rotated image saved to " << outputImagePath << endl;
    } else {
        cerr << "Error saving image" << endl;
        return 1;
    }
    return 0;
}
