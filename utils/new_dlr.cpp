#include <Halide.h>
#include <halide_image_io.h>
#include <iostream>
#include <cmath>

using namespace Halide;
using namespace Halide::Tools;

int main(int argc, char **argv) {
    try {
        std::cout << "Starting the program..." << std::endl;

        std::string inputImagePath = "../Test_Files/test_img_05.png";
        std::string outputImagePath = "../Rotated Images/RN_test_05.png";

        // Load input image
        std::cout << "Loading input image: " << inputImagePath << std::endl;
        Buffer<uint8_t> input = load_image(inputImagePath);

        // Define the Halide function
        Func rotated;
        Var x, y, c;

        // Convert angle to radians
        float theta = M_PI / 4; // 45 degrees in radians
        float cos_theta = cos(theta);
        float sin_theta = sin(theta);

        // Define the rotation transformation
        Expr new_x = cos_theta * (x - input.width() / 2.0f) - sin_theta * (y - input.height() / 2.0f) + input.width() / 2.0f;
        Expr new_y = sin_theta * (x - input.width() / 2.0f) + cos_theta * (y - input.height() / 2.0f) + input.height() / 2.0f;

        // Boundary conditions
        Expr clamped_new_x = clamp(cast<int>(new_x), 0, input.width() - 1);
        Expr clamped_new_y = clamp(cast<int>(new_y), 0, input.height() - 1);

        // Define the function
        rotated(x, y, c) = input(clamped_new_x, clamped_new_y, c);

        // Realize the function
        Buffer<uint8_t> output = rotated.realize({input.width(), input.height(), input.channels()});

        std::cout << "Saving the output image: " << outputImagePath << std::endl;
        // Save the output image
        save_image(output, outputImagePath);

        std::cout << "Rotation applied and saved to " << outputImagePath << std::endl;

        return 0;
    } catch (Halide::CompileError &e) {
        std::cerr << "Halide CompileError: " << e.what() << std::endl;
        return -1;
    } catch (std::exception &e) {
        std::cerr << "Standard Exception: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown Error" << std::endl;
        return -1;
    }
}
