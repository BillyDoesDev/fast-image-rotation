cmake_minimum_required(VERSION 3.5)

project(darotator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILES
    # utils/parallel_rotation.cpp
    # shear_rotation.cpp
    # utils/opencv_demo.cpp
    # dsp_image_rot.cpp
    # utils/subhadeep.cpp
    # utils/opencv_warpAffine.cpp

    utils/dlr_opencv.cpp
    # utils/dlr_opencv_sse.cpp
    # utils/dlr_halide.cpp
    # utils/hermite.cpp
    # utils/rotation_h.cpp

    ### UNCOMMENT FOR AUTOSCHEDULING GENERATION ###
    # autoscheduling.cpp
    # rotate_auto_schedule.cpp
    # Halide-17.0.1-x86-64-linux/share/Halide/tools/GenGen.cpp
    ###############################################

    ### UNCOMMENT FOR AUTOSCHEDULING RUNNING ###
    # autoschedule_run.cpp
    # rotate_auto_run.cpp
    # build/auto_schedule_false.a
    # build/auto_schedule_true.a
    ############################################
)

# Include directories for Halide and its tools
include_directories(${PROJECT_SOURCE_DIR}/Halide-17.0.1-x86-64-linux/include)
include_directories(${PROJECT_SOURCE_DIR}/Halide-17.0.1-x86-64-linux/share/Halide/tools)

# Find packages
find_package(ZLIB REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(OpenCV REQUIRED)
# find_library(DL_LIBRARY dl)

if(NOT JPEG_FOUND)
    message(FATAL_ERROR "JPEG library not found")
endif()

if(NOT PNG_FOUND)
    message(FATAL_ERROR "PNG library not found")
endif()

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link against Halide
target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/Halide-17.0.1-x86-64-linux/lib/libHalide.so)

### UNCOMMENT FOR AUTOSCHEDULING RUNNING ###
# target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/build/auto_schedule_true.a)
# target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/build/auto_schedule_false.a)
############################################

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

target_link_libraries(${PROJECT_NAME} ZLIB::ZLIB)
target_link_libraries(${PROJECT_NAME} JPEG::JPEG)
target_link_libraries(${PROJECT_NAME} PNG::PNG)

target_link_libraries(${PROJECT_NAME} dl)
target_link_libraries(${PROJECT_NAME} pthread)

# add_compile_options(-mavx2)
