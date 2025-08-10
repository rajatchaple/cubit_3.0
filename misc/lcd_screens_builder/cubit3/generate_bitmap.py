import os
from PIL import Image
import numpy as np
from datetime import datetime

def create_bitmap(image_path):
    image = Image.open(image_path).convert('L')  # Convert to grayscale
    image_array = np.array(image)
    bitmap_array = np.where(image_array > 128, 1, 0)  # Threshold for light and dark
    return bitmap_array, image_array.shape

def generate_header_file(folder_path, header_file, source_file):
    header_content = f"""/* 
* lcd_bitmap.h
*
*  Created on: {datetime.now().strftime('%b %d, %Y')}
*      Author: rajat
*/

#ifndef SRC_LCD_BITMAP_H_
#define SRC_LCD_BITMAP_H_

typedef struct {{
    int width;
    int height;
    const char* bitmap;
}} ImageBitmap;

"""

    source_content = f"""/* 
* lcd_bitmap.c
*
*  Created on: {datetime.now().strftime('%b %d, %Y')}
*      Author: rajat
*/

#include "lcd_bitmap.h"

"""

    extern_declarations = ""
    large_digits = []
    small_digits = []

    with open(header_file, 'w') as header, open(source_file, 'w') as source:
        header.write(header_content)
        source.write(source_content)

        for filename in os.listdir(folder_path):
            if filename.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp')):
                image_path = os.path.join(folder_path, filename)
                bitmap_array, (height, width) = create_bitmap(image_path)
                array_name = os.path.splitext(filename)[0]
                
                # Write the bitmap array
                header.write(f'static const char {array_name}_bitmap[] = {{\n')
                for row in bitmap_array:
                    header.write(','.join(map(str, row)) + ',\n')
                header.write('};\n')
                
                # Write the struct for the image
                source.write(f'const ImageBitmap {array_name} = {{\n')
                source.write(f'    {width}, {height}, {array_name}_bitmap\n')
                source.write('};\n\n')

                # Add extern declaration
                extern_declarations += f'extern const ImageBitmap {array_name};\n'

                # Collect digit names if they start with 'l' or 's' followed by a digit
                if array_name[0] == 'l' and array_name[1:].isdigit():
                    large_digits.append(array_name)
                elif array_name[0] == 's' and array_name[1:].isdigit():
                    small_digits.append(array_name)

        # Write extern declarations
        header.write("\n" + extern_declarations)
        # Add large_digits and small_digits extern declarations
        header.write("\nextern const ImageBitmap large_digits[];\n")
        header.write("extern const ImageBitmap small_digits[];\n")
        header.write("#endif /* SRC_LCD_BITMAP_H_ */")

        # Write large_digits and small_digits arrays in source file
        if large_digits:
            source.write(f'const ImageBitmap large_digits[] = {{ {", ".join(large_digits)} }};\n')
        if small_digits:
            source.write(f'const ImageBitmap small_digits[] = {{ {", ".join(small_digits)} }};\n')

# Usage
folder_path = os.path.join(os.getcwd(), 'screen_images')
header_file = 'lcd_bitmap.h'
source_file = 'lcd_bitmap.c'
generate_header_file(folder_path, header_file, source_file)
