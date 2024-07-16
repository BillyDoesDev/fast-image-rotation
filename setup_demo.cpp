#include <arm_neon.h>
#include <opencv2/opencv.hpp>
#include <stdio.h>

uint32x4_t double_elements(uint32x4_t input) {
    return (vaddq_u32(input, input));
}

void print_vector(uint32x4_t vec) {
    // Extract elements from the vector
    uint32_t elements[4];
    vst1q_u32(elements, vec);

    std::cout << "[" << elements[0] << ", " << elements[1] << ", " << elements[2] << ", " << elements[3] << "]" << std::endl;
}

int main(int, char **) {
    printf("Hello, from nuts!\n");
    cv::Mat img = cv::imread("../assets/fish.png");
    if (!img.data) {
        printf("No image data \n");
        return -1;
    }
    cv::imwrite("../outputs/out.png", img);
    std::cout << "done!\n";

    // Initialize a uint32x4_t vector with some values
    uint32_t data[4] = {1, 2, 3, 4};
    uint32x4_t input_vec = vld1q_u32(data);

    std::cout << "Input vector: ";
    print_vector(input_vec);

    uint32x4_t result_vec = double_elements(input_vec);

    std::cout << "Result vector: ";
    print_vector(result_vec);
}
