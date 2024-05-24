cmake_minimum_required(VERSION 3.5)

project(darotator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the source files
set(SOURCE_FILES
    main.cpp
)

# you do a `find_package` when something is installed globally on your machine
# and you do a `include_directories` when you're including that in your project


# Add include directories for Halide
include_directories(${PROJECT_SOURCE_DIR}/Halide-17.0.1-x86-64-linux/include)

# Add include directories for OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Find zlib
find_package(ZLIB REQUIRED)

# Create an executable target
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link against Halide
target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/Halide-17.0.1-x86-64-linux/lib/libHalide.a)

# Link against OpenCV libraries
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

# Link against zlib
target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)
