#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: program input_path output_path angle\n");
        return -1;
    }

    cv::Mat img = cv::imread(argv[1]);

    cv::Point2f center(img.cols / 2.0, img.rows / 2.0);

    double angle = std::stod(argv[3]);
    double scale = 1.0; // Scale factor
    cv::Mat rot_matrix = cv::getRotationMatrix2D(center, angle, scale);

    // Rotate the image using bicubic interpolation
    cv::Mat rotated_img;
    cv::warpAffine(img, rotated_img, rot_matrix, img.size(), cv::INTER_CUBIC);

    cv::imwrite(argv[2], rotated_img);
    // cv::imshow("Rotated Image", rotated_img);
    // cv::waitKey(0);

    return 0;
}
