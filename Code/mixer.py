import numpy as np
from PIL import Image
from PIL import ImageEnhance

#image_path = "./Code/code/Textures/Crystals.png"

original_image = Image.open(image_path)

# Load the darkened image

image = original_image.convert("RGBA")



# Convert the image to a numpy array for pixel manipulation

image_array = np.array(image)



# Define the RGBA blend color and its alpha

blend_color = np.array([0, int(0.462 * 255), int(0.023 * 255), int(0.480 * 255)])

blend_alpha = blend_color[3] / 255.0



# Blend the color with each pixel in the image

blended_array = image_array.copy()

for i in range(3):  # Only blend RGB channels; leave alpha unchanged

    blended_array[..., i] = (

        blend_alpha * blend_color[i] +

        (1 - blend_alpha) * image_array[..., i]

    ).astype(np.uint8)



# Create a new image from the blended array

blended_image = Image.fromarray(blended_array, "RGBA")



# Save and display the result

blended_image_path = "./Code/code/Textures/Crystals.png"

blended_image.save(blended_image_path)

blended_image.show()