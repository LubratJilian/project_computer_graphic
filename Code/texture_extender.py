from PIL import Image
from PIL import ImageEnhance



# Open the uploaded image

image_path = "./texture/Ankama.jpg"

original_image = Image.open(image_path)

s = 12

# Calculate new dimensions for tiling (2x2 grid of the original image)

new_width = original_image.width * s

new_height = original_image.height * s



# Create a new blank image with the calculated dimensions

new_image = Image.new("RGB", (new_width, new_height))



# Paste the original image in a 2x2 grid to achieve the effect of smaller rocks

for i in range(s):
    for j in range(s):

        new_image.paste(original_image, (i * original_image.width, j * original_image.height))



# Save the new image

new_image_path = "./texture/Ankama"+str(s)+".jpg"

enhancer = ImageEnhance.Brightness(new_image)
new_image = enhancer.enhance(0.65)  # 70% brightness


new_image.save(new_image_path)

new_image.show()