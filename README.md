# fast-image-rotation-aarch64
'A fast, parallel image rotation algorithm, based on DLR.

## Setting up the dev environment for arrch64

Install deps</br>
**For Arch Linux:**
```sh
sudo pacman -Sy aarch64-linux-gnu-gcc glew fmt vtk hdf5 qemu-user-static # or whichever version of the aarch64 gcc compiler you're using
```

**For Debian derivatives:**
```sh
sudo apt install -y pkg-config ffmpeg libavcodec-dev libavformat-dev libswscale-dev libgtk-4-dev libdc1394-25 libdc1394-dev libjpeg-dev libpng-dev libtiff-dev jasper gcc-10-aarch64-linux-gnu g++-10-aarch64-linux-gnu qemu-user-static
```

**Get OpenCV, and create a build folder:**
```sh
git clone https://github.com/opencv/opencv.git
cd opencv
mkdir bleh && cd bleh
```

edit the cmake toolchain file like so (`../platforms/linux/aarch64-gnu.toolchain.cmake`):
```cmake
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(GCC_COMPILER_VERSION "" CACHE STRING "GCC Compiler version")
set(GNU_MACHINE "aarch64-linux-gnu" CACHE STRING "GNU compiler triple")

set(CMAKE_C_COMPILER /usr/bin/aarch64-linux-gnu-gcc)        # be sure to set the correct paths for your system
set(CMAKE_CXX_COMPILER /usr/bin/aarch64-linux-gnu-g++)      # be sure to set the correct paths for your system
include("${CMAKE_CURRENT_LIST_DIR}/arm.toolchain.cmake")
```

next, build OpenCV (make sure you're inside the build dir while doing this)
```sh
cmake -DBUILD_SHARED_LIBS=OFF -DSOFTFP=ON -DCMAKE_TOOLCHAIN_FILE=../platforms/linux/aarch64-gnu.toolchain.cmake .. # we need static libs, and a softp version too (yea, doing it without softp makes gcc really mad)
make -j5 # allocate as much as you safely can. go grab a drink, this will take a while .-.
make install
```


This should compile OpenCV for aarch64.

## Building the project

Inside this directory, make a build directory and use `cmake` to build your project like so (make sure you have `cmake` installed and added to your path):
> Note: make sure to adjust your compiler paths accordingly.
```sh
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/aarch64-linux-gnu-gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/aarch64-linux-gnu-g++ --no-warn-unused-cli -S .. -B . -G "Unix Makefiles"
cmake --build . --config Release --target all --
# and then run it with an emulator of your choice (for some reason, it seems to run as is on Ubuntu)
qemu-aarch64-static ./name_of_your_binary
```


## Sample Execution

As of now, the way the project is structured, all the files in the `./utils/` directory get compiled only. The files ending with a `__b` (both cpp files, and their corresponding binaries) signify that these files are being used for benchmarking.

In this repository, the final working file is `./utils/dlr__b.cpp`, and as such, that's the file you should be interested in runing in.

**Usage:**
```sh
path/to/dlr__b [path/to/input/image] [path/to/output/image] [angle_in_degrees]
```
