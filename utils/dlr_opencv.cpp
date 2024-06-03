#include <opencv2/opencv.hpp>
#include <cmath>

cv::Mat rotateImage(const cv::Mat& src, double angle) {
    // Convert angle to radians
    double radians = angle * CV_PI / 180.0;

    // Calculate the size of the output image
    int diagonal = std::sqrt(src.rows * src.rows + src.cols * src.cols);
    cv::Size dstSize(diagonal, diagonal);

    // Center of rotation
    cv::Point2f center(dstSize.width / 2.0f, dstSize.height / 2.0f);

    // Create the transformation matrices for the reflections
    cv::Mat M1 = (cv::Mat_<double>(2, 3) << 0, 1, 0, 1, 0, 0); // Reflect across y = x
    cv::Mat M2 = (cv::Mat_<double>(2, 3) << 0, -1, center.x + center.y, -1, 0, center.x + center.y); // Reflect across y = -x

    // Apply the first reflection
    cv::Mat reflected1;
    cv::warpAffine(src, reflected1, M1, dstSize);

    // Apply the second reflection
    cv::Mat reflected2;
    cv::warpAffine(reflected1, reflected2, M2, dstSize);

    // Return the final rotated image
    return reflected2;
}

int main(int argc, char** argv) {
    // if (argc != 3) {
    //     std::cerr << "Usage: " << argv[0] << " <image_path> <angle>" << std::endl;
    //     return -1;
    // }

    // Load the image
    cv::Mat src = cv::imread("../assets/Lenna_test_image.png");
    if (src.empty()) {
        std::cerr << "Error loading image" << std::endl;
        return -1;
    }

    // Parse the rotation angle
    double angle = 45.0;

    // Rotate the image
    cv::Mat rotated = rotateImage(src, angle);

    // Display the result
    // cv::imshow("Rotated Image", rotated);
    // cv::waitKey(0);
    cv::imwrite("../outputs/dlr_opencv_out.png", rotated);

    return 0;
}
