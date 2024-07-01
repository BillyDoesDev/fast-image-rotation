import matplotlib.pyplot as plt
from math import *
import numpy as np

def rotate(img, rot, x_offset, y_offset, delta_x, delta_y, delta_i, fs, outer_limit, last_px, horizontal, negate_coords=False, flip_range=False):
    # print("rotating...")
    src_i = 0
    shift = 1
    for i in range(outer_limit):
        x_, y_ = fs(i), i
        range_ = range(last_px, -1, -1) if flip_range else range(last_px) 
        for px in range_:
            try:
                x, y, i_ = [floor(_) for _ in (x_, y_, src_i)]
                if negate_coords:
                    shift = -1
                
                a, b = (px, i_) if not horizontal else (i_, px)

                print(f"(a_, b_): {(shift * (y + y_offset), shift * (x + x_offset))} -> (a, b): {(a, b)}")
                rot[shift * (y + y_offset), shift * (x + x_offset)] = img[a, b] # y, x
                rot[shift * (y + y_offset) + 1, shift * (x + x_offset)] = img[a + 1, b + 1]
                x_ += delta_x
                y_ += delta_y
            except IndexError:
                # print("[index err]")
                pass

        src_i += delta_i


def main(fpath, angle):
    img = plt.imread(fpath)
    # try:
    #     img = plt.imread("./assets/fish_1080.png")
    # except FileNotFoundError:
    #     img = plt.imread("../assets/fish_1080.png")

    m, n = img.shape[:2]

    # angle = 330
    # for angle in range(361):
    angle = angle - (angle // 360) * 360
    if angle < 0 : angle += 360
    alpha = (pi * angle) / 180

    sin_alpha = abs(sin(alpha))
    cos_alpha = abs(cos(alpha))
    tan_alpha = tan(alpha)
    tan_alpha_ = tan(alpha + pi / 2)

    mrt = ceil(m * cos_alpha + n * sin_alpha)
    nrt = ceil(m * sin_alpha + n * cos_alpha)
    rot = np.zeros((mrt, nrt, 4), dtype=img.dtype)

    # print(f"{(m, n, mrt, nrt) = }")

    # zone 1 and 5
    if (z1 := 0 <= angle <= 45) or (180 < angle <= 225):
        x_offset = ceil(m * sin_alpha)
        y_offset = 0
        
        fs = lambda y: y / tan_alpha_   # starting line equation
        delta_x = cos_alpha
        delta_y = sin_alpha
        delta_i = 1 / delta_x

        outer_limit = ceil(m * cos_alpha)
        last_px = n
        rotate(img, rot, x_offset, y_offset, delta_x, delta_y, delta_i, fs, outer_limit, last_px, horizontal=True, negate_coords=not z1)
    
    # zone 2 and 6
    elif (z2 := 45 < angle <= 90) or (225 < angle <= 270):
        x_offset = ceil(m * sin_alpha)
        y_offset = 0
        
        fs = lambda y: y / tan_alpha    # starting line equation
        delta_x = -sin_alpha
        delta_y = cos_alpha
        delta_i = (-1 if z2 else 1) / delta_x

        outer_limit = ceil(n * sin_alpha)
        last_px = m
        rotate(img, rot, x_offset, y_offset, delta_x, delta_y, delta_i, fs, outer_limit, last_px, horizontal=False, flip_range=not z2)
    
    # zone 3 and 7
    elif (z2 := 90 < angle <= 135) or (270 < angle <= 315):
        x_offset = nrt
        y_offset = ceil(m * cos_alpha)
        
        fs = lambda y: y / tan_alpha    # starting line equation
        delta_x = -sin_alpha
        delta_y = -cos_alpha
        delta_i = (-1 if z2 else 1) / delta_x

        outer_limit = ceil(n * sin_alpha)
        last_px = m
        rotate(img, rot, x_offset, y_offset, delta_x, delta_y, delta_i, fs, outer_limit, last_px, horizontal=False, flip_range=not z2)
    
    # zone 4 and 8
    elif (z1 := 135 < angle <= 180) or (315 < angle <= 360):
        x_offset = ceil(n * cos_alpha)
        y_offset = 0
        
        fs = lambda y: y / tan_alpha_   # starting line equation
        delta_x = -cos_alpha
        delta_y = sin_alpha
        delta_i = 1 / delta_x

        outer_limit = ceil(m * cos_alpha)
        last_px = n
        rotate(img, rot, x_offset, y_offset, delta_x, delta_y, delta_i, fs, outer_limit, last_px, horizontal=True, negate_coords=not z1)


    plt.figure(figsize=(10, 5))
    plt.subplot(1, 2, 1)
    plt.title("Original Image")
    plt.imshow(img, cmap="gray")

    plt.subplot(1, 2, 2)
    plt.title("Rotated Image")
    plt.imshow(rot, cmap="gray")
    # plt.show()
    plt.savefig("lol.png")
    plt.close()


if __name__ == "__main__":
    # for angle in range(361):
    #     try:
    #         main("assets/fish_1080.png", angle)
    #     except e:
    #         print(f"[{angle}] deg, {e}")
    main("assets/fish_2880.png", 226)