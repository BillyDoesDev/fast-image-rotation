#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: program input_path output_path angle\n");
        return -1;
    }

    Mat img = imread(argv[1]);

    double rot_angle = -stod(argv[3]);
    Point2f center(img.cols / 2.0, img.rows / 2.0);

    Mat rotation_mat = getRotationMatrix2D(center, rot_angle, 1.0);
    
    // Calculate the size of the bounding box for the rotated image
    Rect2f bbox = RotatedRect(Point2f(), img.size(), rot_angle).boundingRect2f();

    // Adjust the transformation matrix
    rotation_mat.at<double>(0, 2) += bbox.width / 2.0 - img.cols / 2.0;
    rotation_mat.at<double>(1, 2) += bbox.height / 2.0 - img.rows / 2.0;

    Mat rotated;
    warpAffine(img, rotated, rotation_mat, bbox.size(), INTER_LINEAR);

    imwrite(argv[2], rotated);

    if (argc > 4) {
        // file_to_compare input_dump_file output_dump_file
        img = imread(argv[4]);
        FILE *fptr;
        fptr = fopen(argv[5], "w");
        for (int y = 0; y < img.rows; y++) {
            for (int x = 0; x < img.cols; x++) {
                auto px = img.at<cv::Vec3b>(y, x);
                fprintf(fptr, "%d ", (px[0] + px[1] + px[2]) / 3);
            }
            fprintf(fptr, "\n");
        }
        fclose(fptr);

        fptr = fopen(argv[6], "w");
        for (int y = 0; y < rotated.rows; y++) {
            for (int x = 0; x < rotated.cols; x++) {
                auto px = rotated.at<cv::Vec3b>(y, x);
                fprintf(fptr, "%d ", (px[0] + px[1] + px[2]) / 3);
            }
            fprintf(fptr, "\n");
        }
        fclose(fptr);
    }
    return 0;
}
