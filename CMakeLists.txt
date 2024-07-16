cmake_minimum_required(VERSION 3.5)
project(nuts VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Get all the cpp files in the utils directory
file(GLOB SOURCE_FILES "utils/*.cpp")

set(CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/opencv/bleh/install)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Create separate targets for each cpp file
foreach(SOURCE_FILE ${SOURCE_FILES})
    # Get the file name without extension
    get_filename_component(TARGET_NAME ${SOURCE_FILE} NAME_WE)
    add_executable(${TARGET_NAME} ${SOURCE_FILE})
    target_link_libraries(${TARGET_NAME} ${OpenCV_LIBS})
endforeach()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -O3 -fopenmp")
