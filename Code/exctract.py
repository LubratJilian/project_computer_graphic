import cv2

# Load the image
image = cv2.imread('material_baseColor.jpeg')

# Convert to grayscale
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

# Apply thresholding to remove the white background
_, thresh = cv2.threshold(gray, 240, 255, cv2.THRESH_BINARY_INV)

# Find contours of the objects
contours, _ = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

# Loop through each contour and save the cropped images
for i, contour in enumerate(contours):
    x, y, w, h = cv2.boundingRect(contour)
    cropped_image = image[y:y+h, x:x+w]
    cv2.imwrite(f'extracted_image_{i}.jpg', cropped_image)

print("Images extracted and saved.")