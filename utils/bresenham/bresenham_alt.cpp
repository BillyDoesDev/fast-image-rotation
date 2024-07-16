#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

// Function to rotate a single channel of an image by a given angle using backward mapping and Bresenham's algorithm for line scanning
cv::Mat rotateChannel(const cv::Mat &channel, double angle) {
    int h = channel.rows;
    int w = channel.cols;
    double dc = std::cos(angle * CV_PI / 180.0);
    double ds = std::sin(angle * CV_PI / 180.0);

    // Calculate dimensions of the new image
    int h_new = static_cast<int>(std::abs(h * dc) + std::abs(w * ds));
    int w_new = static_cast<int>(std::abs(h * ds) + std::abs(w * dc));

    cv::Mat rotatedChannel = cv::Mat::zeros(h_new, w_new, channel.type());

    int cx = w / 2;
    int cy = h / 2;
    int cx_new = w_new / 2;
    int cy_new = h_new / 2;

    for (int y_new = 0; y_new < h_new; ++y_new) {
        for (int x_new = 0; x_new < w_new; ++x_new) {
            // Map the destination pixel back to the source image
            int x = static_cast<int>((x_new - cx_new) * dc + (y_new - cy_new) * ds + cx);
            int y = static_cast<int>(-(x_new - cx_new) * ds + (y_new - cy_new) * dc + cy);

            if (x >= 0 && x < w && y >= 0 && y < h) {
                rotatedChannel.at<uchar>(y_new, x_new) = channel.at<uchar>(y, x);
            }
        }
    }

    return rotatedChannel;
}

// Function to rotate a color image by a given angle
cv::Mat rotateImage(const cv::Mat &image, double angle) {
    std::vector<cv::Mat> channels;
    cv::split(image, channels);

    std::vector<cv::Mat> rotatedChannels;
    for (auto &channel : channels) {
        rotatedChannels.push_back(rotateChannel(channel, angle));
    }

    cv::Mat rotatedImage;
    cv::merge(rotatedChannels, rotatedImage);

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
    //  std::cout << "Image rotated by " << angle << " degrees and saved to " << outputImagePath << std::endl;

    // Display original and rotated images
    // cv::imshow("Original Image", image);
    // cv::imshow("Rotated Image", rotatedImage);
    // cv::waitKey(0);

    return 0;
}