#include <opencv2/opencv.hpp>
#include <cmath>
#include <iostream>
#include <vector>

// Function to generate points of a line using Bresenham's algorithm
std::vector<cv::Point> bresenhamLine(int x0, int y0, int x1, int y1) {
    std::vector<cv::Point> points;
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        points.push_back(cv::Point(x0, y0));
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
    return points;
}

// Function to rotate an image by a given angle using Bresenham's algorithm for line scanning
cv::Mat rotateImage(const cv::Mat& image, double angle) {
    int h = image.rows;
    int w = image.cols;
    double dc = std::cos(angle * CV_PI / 180.0);
    double ds = std::sin(angle * CV_PI / 180.0);

    // Calculate dimensions of the new image
    int h_new = static_cast<int>(std::abs(h * dc) + std::abs(w * ds));
    int w_new = static_cast<int>(std::abs(h * ds) + std::abs(w * dc));

    cv::Mat rotatedImage = cv::Mat::zeros(h_new, w_new, image.type());

    int cx = w / 2;
    int cy = h / 2;
    int cx_new = w_new / 2;
    int cy_new = h_new / 2;

    for (int y = 0; y < h; ++y) {
        auto line = bresenhamLine(0, y, w - 1, y); // Get points of the line using Bresenham's algorithm
        for (const auto& point : line) {
            int x = point.x;
            int y = point.y;
            int x_new = static_cast<int>((x - cx) * dc - (y - cy) * ds + cx_new);
            int y_new = static_cast<int>((x - cx) * ds + (y - cy) * dc + cy_new);

            if (x_new >= 0 && x_new < w_new && y_new >= 0 && y_new < h_new) {
                rotatedImage.at<cv::Vec3b>(y_new, x_new) = image.at<cv::Vec3b>(y, x);
            }
        }
    }

    // Fill the gaps using nearest-neighbor interpolation
    for (int y_new = 0; y_new < h_new; ++y_new) {
        for (int x_new = 0; x_new < w_new; ++x_new) {
            if (rotatedImage.at<cv::Vec3b>(y_new, x_new) == cv::Vec3b(0, 0, 0)) {
                int x_nearest = static_cast<int>((x_new - cx_new) * dc + (y_new - cy_new) * ds + cx);
                int y_nearest = static_cast<int>(-(x_new - cx_new) * ds + (y_new - cy_new) * dc + cy);

                if (x_nearest >= 0 && x_nearest < w && y_nearest >= 0 && y_nearest < h) {
                    rotatedImage.at<cv::Vec3b>(y_new, x_new) = image.at<cv::Vec3b>(y_nearest, x_nearest);
                }
            }
        }
    }

    return rotatedImage;
}

int main(int argc, char *argv[]) {

    if (argc < 4) {
        printf("Usage: program input_path output_path angle\n");
        return -1;
    }

    std::string inputImagePath = argv[1];
    std::string outputImagePath = argv[2];

    cv::Mat image = cv::imread(inputImagePath, cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cerr << "Could not open or find the image!\n";
        return -1;
    }

    double angle = std::stoi(argv[3]); // Rotate by 45 degrees
    cv::Mat rotatedImage = rotateImage(image, angle);

    cv::imwrite(outputImagePath, rotatedImage);
    // std::cout << "Image rotated by " << angle << " degrees and saved to " << outputImagePath << std::endl;

    // Display original and rotated images
    // cv::imshow("Original Image", image);
    // cv::imshow("Rotated Image", rotatedImage);
    // cv::waitKey(0);

    return 0;
}