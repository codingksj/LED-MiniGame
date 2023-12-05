import cv2
import numpy as np

# Input parameters
data_type = input("자료형 입력: ")
array_name = input("배열명 입력: ")

# Read the image
image = cv2.imread('texts.png', cv2.IMREAD_GRAYSCALE)

# Find black pixel locations
black_pixel_locations = np.column_stack(np.where(image == 0))

# Create a text file to write the C array
output_file_path = f"{array_name}.txt"

with open(output_file_path, 'w') as output_file:
    output_file.write(f"{data_type} {array_name}[] = {{\n")

    count = 0
    for i in range(image.shape[0]):
        for j in range(image.shape[1]):
            if image[i, j] == 0:
                if count % 10 == 0:
                    output_file.write(f"    ")
                output_file.write(f"{i * image.shape[1] + j}, ")
                count += 1

                # Add a newline character every 10 elements
                
                if count % 10 == 0:
                    output_file.write("\n")
            else:
                pass  

    output_file.write("\n};")

print(f"C array information written to {output_file_path}")
