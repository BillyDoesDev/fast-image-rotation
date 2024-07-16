import matplotlib.pyplot as plt
from math import *
import numpy as np

try:
    img = plt.imread("./assets/fish_1080.png")
except FileNotFoundError:
    img = plt.imread("../assets/fish_1080.png")

m, n = img.shape[:2]

angle = 45 # 45 to 90

# for index, angle in enumerate(range(45, 120)):
alpha = (pi * angle) / 180

mrt = ceil(abs(m * cos(alpha)) + abs(n * sin(alpha)))
nrt = ceil(abs(m * sin(alpha)) + abs(n * cos(alpha)))

x_offset = ceil(m * sin(alpha))
fs = lambda y: y / tan(alpha)                       # starting line equation

rot = np.zeros((mrt, nrt, 4), dtype=img.dtype)

delta_x = abs(sin(alpha))
delta_y = abs(cos(alpha))
delta_i = 1 / delta_x

poopie = 0
for i in range(ceil(n * sin(alpha))): # as this i goes from 0 to ..., we need the source image's i to go from 0 to n
    x_, y_ = fs(i), i
    
    for px in range(m):
        try:
            x, y, pp = floor(x_), floor(y_), floor(poopie)
            rot[y, x + x_offset] = img[px, floor(poopie)] # y, x
            rot[y + 1, x + x_offset] = img[px + 1, floor(poopie) + 1]
            x_ -= delta_x
            y_ += delta_y
        except IndexError:
            # print(f"[index err] mapping {(x + x_offset, y)} to {(i, px)}")
            pass
    poopie += delta_i

plt.figure(figsize=(10, 5))
plt.subplot(1, 2, 1)
plt.title("Original Image")
plt.imshow(img, cmap="gray")

plt.subplot(1, 2, 2)
plt.title(f"{(round(delta_x, 3), round(delta_y, 3)) = }")
plt.imshow(rot, cmap="gray")
plt.show()
# plt.savefig(f"outputs/dlrz2_{str(angle).rjust(3, '0')}.png")
