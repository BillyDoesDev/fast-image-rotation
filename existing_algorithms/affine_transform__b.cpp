// #include <iostream>
// #include <opencv2/opencv.hpp>

// cv::Mat rotateImageNearestNeighbor(const cv::Mat& src, double angle) {
//     cv::Point2f center(src.cols / 2.0, src.rows / 2.0);
//     cv::Mat rotMatrix = cv::getRotationMatrix2D(center, angle, 1.0);
//     cv::Mat dst;
//     cv::warpAffine(src, dst, rotMatrix, src.size(), cv::INTER_NEAREST);
//     return dst;
// }

// cv::Mat rotateImageBilinear(const cv::Mat& src, double angle) {
//     cv::Point2f center(src.cols / 2.0, src.rows / 2.0);
//     cv::Mat rotMatrix = cv::getRotationMatrix2D(center, angle, 1.0);
//     cv::Mat dst;
//     cv::warpAffine(src, dst, rotMatrix, src.size(), cv::INTER_LINEAR);
//     return dst;
// }

// cv::Mat rotateImageBicubic(const cv::Mat& src, double angle) {
//     cv::Point2f center(src.cols / 2.0, src.rows / 2.0);
//     cv::Mat rotMatrix = cv::getRotationMatrix2D(center, angle, 1.0);
//     cv::Mat dst;
//     cv::warpAffine(src, dst, rotMatrix, src.size(), cv::INTER_CUBIC);
//     return dst;
// }

// cv::Mat rotateImageAffine(const cv::Mat& src, double angle) {
//     // double angleRad = angle * CV_PI / 180.0;
//     // cv::Mat rotationMatrix = (cv::Mat_<double>(2, 3) <<
//     //     cos(angleRad), -sin(angleRad), 0,
//     //     sin(angleRad), cos(angleRad), 0);

//     // cv::Mat dst;
//     // cv::warpAffine(src, dst, rotationMatrix, src.size(), cv::INTER_LINEAR);
//     // return dst;

//     cv::Point2f center(src.cols / 2.0, src.rows / 2.0);
//     cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, angle, 1.0);
//     cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), src.size(), angle).boundingRect2f();
//     rotationMatrix.at<double>(0, 2) += bbox.width / 2.0 - center.x;
//     rotationMatrix.at<double>(1, 2) += bbox.height / 2.0 - center.y;

//     cv::Mat dst;
//     cv::warpAffine(src, dst, rotationMatrix, bbox.size());

//     return dst;
// }

// // cv::Mat rotateImage90Degrees(const cv::Mat& src, int rotationCode) {
// //     cv::Mat dst;
// //     cv::rotate(src, dst, rotationCode);  // rotationCode can be cv::ROTATE_90_CLOCKWISE, cv::ROTATE_90_COUNTERCLOCKWISE, or cv::ROTATE_180
// //     return dst;
// // }

// int main(int argc, char *argv[]) {
//     cv::Mat image = cv::imread(argv[1]);
//     if (image.empty()) {
//         std::cerr << "Image not found!" << std::endl;
//         return -1;
//     }

//     double angle = 45.0;  // Rotate 45 degrees

//     cv::Mat rotatedNN = rotateImageNearestNeighbor(image, angle);
//     cv::Mat rotatedBL = rotateImageBilinear(image, angle);
//     cv::Mat rotatedBC = rotateImageBicubic(image, angle);
//     cv::Mat rotatedAffine = rotateImageAffine(image, angle);

//     cv::imwrite("Nearest Neighbor.png", rotatedNN);
//     cv::imwrite("Bilinear.png", rotatedBL);
//     cv::imwrite("Bicubic.png", rotatedBC);
//     cv::imwrite("Affine.png", rotatedAffine);

//     // cv::waitKey(0);
//     return 0;
// }


#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

Mat rotateImage(const Mat& src, double angle) {
    // Convert angle to radians
    double rad = angle * CV_PI / 180.0;
    double cosTheta = cos(rad);
    double sinTheta = sin(rad);

    // Calculate new image bounds
    int newWidth = int(abs(src.cols * cosTheta) + abs(src.rows * sinTheta));
    int newHeight = int(abs(src.cols * sinTheta) + abs(src.rows * cosTheta));

    // Center offsets
    Point2f srcCenter(src.cols / 2.0f, src.rows / 2.0f);
    Point2f dstCenter(newWidth / 2.0f, newHeight / 2.0f);

    // Create new image
    Mat dst = Mat::zeros(newHeight, newWidth, src.type());

    // Iterate over every pixel in the destination image
    for (int y = 0; y < newHeight; ++y) {
        for (int x = 0; x < newWidth; ++x) {
            // Calculate corresponding source coordinates
            int srcX = static_cast<int>((x - dstCenter.x) * cosTheta + (y - dstCenter.y) * sinTheta + srcCenter.x);
            int srcY = static_cast<int>(-(x - dstCenter.x) * sinTheta + (y - dstCenter.y) * cosTheta + srcCenter.y);

            // Check if the source pixel is within bounds
            if (srcX >= 0 && srcX < src.cols && srcY >= 0 && srcY < src.rows) {
                dst.at<Vec3b>(y, x) = src.at<Vec3b>(srcY, srcX);
            }
        }
    }

    return dst;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: program input_path output_path angle\n");
        return -1;
    }
    Mat img = imread(argv[1]);
    
    // Rotate image
    double angle = stod(argv[3]); // Angle in degrees
    Mat rotatedImg = rotateImage(img, angle);
    
    // Display images
    // imshow("Original Image", img);
    // imshow("Rotated Image", rotatedImg);
    
    // Save rotated image
    imwrite(argv[2], rotatedImg);
    
    // waitKey(0);
    return 0;
}
