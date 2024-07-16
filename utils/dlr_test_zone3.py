import matplotlib.pyplot as plt
from math import *
import numpy as np

try:
    img = plt.imread("./assets/fish.png")
except FileNotFoundError:
    img = plt.imread("../assets/fish.png")

m, n = img.shape[:2]

angle = 130 # 90 to 135
alpha = (pi * angle) / 180

mrt = ceil(abs(m * cos(alpha)) + abs(n * sin(alpha)))
nrt = ceil(abs(m * sin(alpha)) + abs(n * cos(alpha)))

# we can get away with the deviation in the values by a check later inside the loop
x_offset = nrt - 2
y_offset = ceil(abs(m * cos(alpha))) - 2
print(f"{(m, n, mrt, nrt, x_offset, y_offset, ceil(abs(n * sin(alpha)))) = }")

fs = lambda y: y / tan(alpha)                       # starting line equation

rot = np.zeros((mrt, nrt), dtype=img.dtype)

delta_x = abs(sin(alpha))
delta_y = abs(cos(alpha))

for i in range(ceil(abs(n * sin(alpha)))):
    x_, y_ = fs(i), i # starting point
    
    for px in range(m): # plot all m pixels 
        try:
            x, y = round(x_), round(y_)
            rot[y + y_offset, x + x_offset] = img[px, i]
            rot[y + y_offset + 1, x + x_offset] = img[px, i]
            x_ -= delta_x
            y_ -= delta_y
        except IndexError:
            print(f"[index err] mapping {(x + x_offset, y + y_offset)} to {(i, px)}")
            pass


plt.figure(figsize=(10, 5))
plt.subplot(1, 2, 1)
plt.title("Original Image")
plt.imshow(img, cmap="gray")

plt.subplot(1, 2, 2)
plt.title("Rotated Image")
plt.imshow(rot, cmap="gray")
plt.show()