# Fast Parallel Image Rotation Algorithm

**Abstract**</br>
yes.

## Setup and building

- Install OpenCV (this is only temporary, since we'll use this to read and write images only for now, and later on will switch to [Halide](http://halide-lang.org))
    ```sh
    # the following instructions are for arch
    # packages and procedures may vary
    sudo pacman -Sy glew fmt vtk hdf5 opencv

    # optionally install `zlib` for Halide in case your system doesn't already have it
    ```

- Clone this repo, download and extract the [binary release of Halide](https://github.com/halide/Halide/releases) for your system and place it in the current working directory.
    > **Note:** You may have to modify the `CMakeLists.txt` file accordingly.

    ```make
    # Add include directories for Halide
    include_directories(${PROJECT_SOURCE_DIR}/Halide-17.0.1-x86-64-linux/include)
    ```
    Also, make sure you have the `zlib` and `OpenCV` binaries in your path.

- Build the thing
    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```

## How it works

**Stage 1: The naive rotation matrix approach**

- First, we calculate the new dimensions of the image to fit the rotation.</br>
<img src="https://raw.githubusercontent.com/BillyDoesDev/fast-image-rotation/main/assets/resized_image.png" /></br>
*the figure above represents the new size of the image after rotation by an angle $\theta$*

- Next, we decide on a centre for rotation (in this case, the centre of the image $(cx, xy)$), and apply the rotation matrix for each pixel coordinate, $(x, y)$ of the image
    $$\begin{bmatrix}
    x'\\ 
    y'
    \end{bmatrix} = \begin{bmatrix}
    x - cx\\ 
    y - cy
    \end{bmatrix} * \begin{bmatrix}
    cos(\theta) & -sin(\theta)\\
    sin(\theta) & cos(\theta)
    \end{bmatrix}$$

    Assuming the centre of rotation of the new image is $(cx', cy')$, the final new coordinates are:
    $$\begin{bmatrix}
    x'\\ 
    y'
    \end{bmatrix} = \begin{bmatrix}
    x' + cx'\\ 
    y' + cy'
    \end{bmatrix}$$

- Finally, we do `pixel_val_at(x', y') = pixel_val_at(x, y)`


## Issues:

A lot, but mostly, for now, anti-aliasing. This means, we'll most likely be looking into a multi-pass program.
