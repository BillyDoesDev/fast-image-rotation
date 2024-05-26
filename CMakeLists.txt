cmake_minimum_required(VERSION 3.5)

project(darotator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the source files
set(SOURCE_FILES
    rotation_copy.cpp
)

# Add include directories for Halide
include_directories(${PROJECT_SOURCE_DIR}/Halide-17.0.1-x86-64-linux/include)

# Add include directories for Halide tools (if needed)
include_directories(${PROJECT_SOURCE_DIR}/Halide-17.0.1-x86-64-linux/share/Halide/tools)

# Add include directories for OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Find zlib
find_package(ZLIB REQUIRED)

# Find JPEG library
find_package(JPEG REQUIRED)
include_directories(${JPEG_INCLUDE_DIR})

# Find PNG library
find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})

# Create an executable target
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link against Halide
target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/Halide-17.0.1-x86-64-linux/lib/libHalide.a)

# Link against OpenCV libraries
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

# Link against zlib
target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)

# Link against JPEG library
target_link_libraries(${PROJECT_NAME} ${JPEG_LIBRARIES})

# Link against PNG library
target_link_libraries(${PROJECT_NAME} ${PNG_LIBRARIES})

# Link against threading library if needed
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} Threads::Threads)
