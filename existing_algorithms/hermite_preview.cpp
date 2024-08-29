// #include <iostream>
// #include <cmath>
// #include <vector>
// #include <opencv2/opencv.hpp>

// cv::Mat rotateImageHermite(const cv::Mat& image, double angle) {
//     // Convert angle to radians
//     double angleRad = angle * M_PI / 180.0;

//     // Original image dimensions
//     int width = image.cols;
//     int height = image.rows;

//     // Calculate the new image dimensions
//     int newWidth = static_cast<int>(std::ceil(std::abs(width * std::cos(angleRad)) + std::abs(height * std::sin(angleRad))));
//     int newHeight = static_cast<int>(std::ceil(std::abs(height * std::cos(angleRad)) + std::abs(width * std::sin(angleRad))));

//     // Calculate the centers of the original and new images
//     cv::Point2f centerOriginal(width / 2.0, height / 2.0);
//     cv::Point2f centerNew(newWidth / 2.0, newHeight / 2.0);

//     // Create the output image
//     cv::Mat output(newHeight, newWidth, image.type(), cv::Scalar(0));

//     for (int y = 0; y < newHeight; ++y) {
//         for (int x = 0; x < newWidth; ++x) {
//             // Calculate coordinates relative to the center of the new image
//             double dx = x - centerNew.x;
//             double dy = y - centerNew.y;

//             // Map the coordinates back to the original image space
//             double originalX = dx * std::cos(angleRad) + dy * std::sin(angleRad) + centerOriginal.x;
//             double originalY = -dx * std::sin(angleRad) + dy * std::cos(angleRad) + centerOriginal.y;

//             // Check if the coordinates are within the bounds of the original image
//             if (originalX >= 0 && originalX < width && originalY >= 0 && originalY < height) {
//                 // Output image to hold the interpolated value
//                 cv::Mat interpolatedPixel;

//                 // Use cv::getRectSubPix to obtain the interpolated value
//                 cv::getRectSubPix(image, cv::Size(1, 1), cv::Point2f(originalX, originalY), interpolatedPixel);

//                 // Assign the interpolated value to the output image
//                 output.at<uchar>(y, x) = interpolatedPixel.at<uchar>(0, 0);
//             }
//         }
//     }
    
//     return output;
// }

// int main() {
//     // Load an image in grayscale
//     cv::Mat image = cv::imread("assets/fish_1080.png", cv::IMREAD_GRAYSCALE);

//     // Rotate the image by 45 degrees
//     cv::Mat rotatedImage = rotateImageHermite(image, 45.0);

//     // Display the result
//     cv::imwrite("rotated_image.png", rotatedImage);

//     return 0;
// }


#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <vector>

using namespace cv;
using namespace std;

class RectBivariateSpline {
public:
    RectBivariateSpline(const Mat& image) : image(image) {}

    double interpolate(double x, double y) const {
        int ix = static_cast<int>(x);
        int iy = static_cast<int>(y);

        if (ix < 1 || ix >= image.cols - 2 || iy < 1 || iy >= image.rows - 2) {
            return 0.0;
        }

        double result = 0.0;
        for (int m = -1; m <= 2; ++m) {
            double arr[4];
            for (int n = -1; n <= 2; ++n) {
                arr[n + 1] = static_cast<double>(image.at<uchar>(iy + m, ix + n));
            }
            result += cubicInterpolate(y - iy, arr) * cubicBasis(x - ix, m);
        }
        return result;
    }

private:
    const Mat& image;

    double cubicInterpolate(double t, double p[4]) const {
        return p[1] + 0.5 * t * (p[2] - p[0] + t * (2.0 * p[0] - 5.0 * p[1] + 4.0 * p[2] - p[3] + t * (3.0 * (p[1] - p[2]) + p[3] - p[0])));
    }

    double cubicBasis(double t, int n) const {
        switch (n) {
            case -1: return ((-t + 2) * t - 1) * t / 2;
            case 0:  return (((3 * t - 5) * t) * t + 2) / 2;
            case 1:  return ((-3 * t + 4) * t + 1) * t / 2;
            case 2:  return ((t - 1) * t * t) / 2;
            default: return 0.0;
        }
    }
};

Mat rotateImageSpline(const Mat& image, double angle) {
    double angleRad = angle * CV_PI / 180.0;
    int h = image.rows;
    int w = image.cols;

    int new_w = static_cast<int>(ceil(abs(w * cos(angleRad)) + abs(h * sin(angleRad))));
    int new_h = static_cast<int>(ceil(abs(h * cos(angleRad)) + abs(w * sin(angleRad))));

    Point2f originalCenter(w / 2.0f, h / 2.0f);
    Point2f newCenter(new_w / 2.0f, new_h / 2.0f);

    Mat output = Mat::zeros(new_h, new_w, image.type());

    RectBivariateSpline spline(image);

    for (int y = 0; y < new_h; ++y) {
        for (int x = 0; x < new_w; ++x) {
            Point2f xyAdjusted(x - newCenter.x, y - newCenter.y);

            double originalX = xyAdjusted.x * cos(angleRad) + xyAdjusted.y * sin(angleRad) + originalCenter.x;
            double originalY = -xyAdjusted.x * sin(angleRad) + xyAdjusted.y * cos(angleRad) + originalCenter.y;

            if (originalX >= 0 && originalX < w && originalY >= 0 && originalY < h) {
                output.at<uchar>(y, x) = static_cast<uchar>(spline.interpolate(originalX, originalY));
            }
        }
    }

    return output;
}

int main() {
    Mat image = imread("assets/fish_360.png", IMREAD_GRAYSCALE);
    if (image.empty()) {
        cerr << "Error loading image" << endl;
        return -1;
    }

    Mat rotatedImage = rotateImageSpline(image, 45);

    imwrite("rotated_image.png", rotatedImage);
    return 0;
}
