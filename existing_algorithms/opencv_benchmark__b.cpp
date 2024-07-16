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

    double rot_angle = stod(argv[3]);
    Point2f center(img.cols / 2.0, img.rows / 2.0);

    Mat rotation_mat = getRotationMatrix2D(center, rot_angle, 1.0);
    
    // Calculate the size of the bounding box for the rotated image
    Rect2f bbox = RotatedRect(Point2f(), img.size(), rot_angle).boundingRect2f();

    // Adjust the transformation matrix
    rotation_mat.at<double>(0, 2) += bbox.width / 2.0 - img.cols / 2.0;
    rotation_mat.at<double>(1, 2) += bbox.height / 2.0 - img.rows / 2.0;

    Mat rotated;
    warpAffine(img, rotated, rotation_mat, bbox.size());

    imwrite(argv[2], rotated);
    return 0;
}
