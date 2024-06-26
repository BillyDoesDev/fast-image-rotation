// #include <ctime>
// #include <fstream>
// #include <iostream>
// #include <time.h>
// #include <chrono>

#include "Halide.h"
#include "halide_image_io.h"

using namespace Halide;

int main() {
    Func gradient_fast("gradient_fast");
    Var x, y;
    gradient_fast(x, y) = x + y;

    // Var x_outer, y_outer, x_inner, y_inner, tile_index;
    // gradient_fast
    //     .tile(x, y, x_outer, y_outer, x_inner, y_inner, 64, 64)
    //     .fuse(x_outer, y_outer, tile_index)
    //     .parallel(tile_index);

    // Var x_inner_outer, y_inner_outer, x_vectors, y_pairs;
    // gradient_fast
    //     .tile(x_inner, y_inner, x_inner_outer, y_inner_outer, x_vectors, y_pairs, 4, 2)
    //     .vectorize(x_vectors)
    //     .unroll(y_pairs);

    // auto t1 = std::chrono::high_resolution_clock::now();

    // Var x_outer, y_outer, x_inner, y_inner, tile_index;
    // gradient_fast
    //     .tile(x, y, x_outer, y_outer, x_inner, y_inner, 32, 32) // Smaller tile size
    //     .fuse(x_outer, y_outer, tile_index)
    //     .parallel(tile_index, 8); // Limit to 8 threads

    // gradient_fast
    //     .vectorize(x_inner, 8); // Vectorize inner loop

    Buffer<int> result = gradient_fast.realize({20000, 20000});
    
    // auto t2 = std::chrono::high_resolution_clock::now();
    // auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    // std::cout << "[L2] Time elapsed: "
            //   << dur << " ms\n";

    //// // // writing result to file
    // std::ofstream outFile("gradient.txt");
    // for (int y = 0; y < result.height(); y++) {
    //     for (int x = 0; x < result.width(); x++) {
    //         outFile << (int)result(x, y) << " ";
    //     }
    //     outFile << "\n";
    // }
    // outFile.close();

    // printf("Pseudo-code for the schedule:\n");
    // gradient_fast.print_loop_nest();
    // printf("\n");

    return 0;
}
