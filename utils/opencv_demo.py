# Python program to explain cv2.rotate() method

# importing cv2
import cv2

src = cv2.imread("../assets/Lenna_test_image.png")
image = cv2.rotate(src, cv2.ROTATE_90_CLOCKWISE)

cv2.imshow("breh", image)
cv2.waitKey(0)
