import os
from PIL import Image
import numpy as np
from datetime import datetime

def create_bitmap(image_path):
    image = Image.open(image_path).convert('1')  # Convert to 1-bit monochrome
    image_array = np.array(image, dtype=np.uint8)
    # Convert image array to bitmap (0 for black, 1 for white)
    bitmap_array = np.where(image_array == 0, 0, 1)
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

    with open(header_file, 'w') as header, open(source_file, 'w') as source:
        header.write(header_content)
        source.write(source_content)

        for filename in os.listdir(folder_path):
            if filename.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp')):
                image_path = os.path.join(folder_path, filename)
                bitmap_array, (height, width) = create_bitmap(image_path)
                array_name = os.path.splitext(filename)[0]

                # Calculate padded width
                padded_width = (width + 7) // 8 * 8

                # Convert the bitmap array to the required format
                byte_list = []
                for row in bitmap_array:
                    bits = ''.join(map(str, row))
                    # Pad bits to make them a multiple of 8
                    bits = bits.ljust(padded_width, '0')
                    byte_list.extend([int(bits[i:i+8], 2) for i in range(0, len(bits), 8)])

                # Debug: Print the bitmap array for verification
                print(f"Bitmap for {array_name} (size: {width}x{height}):")
                for row in bitmap_array:
                    print(''.join(str(bit) for bit in row))

                # Write the bitmap array in the header file
                header.write(f'static const char {array_name}_bitmap[] = {{\n')
                for i in range(0, len(byte_list), 8):  # Group by 8 bytes per line for readability
                    line = ', '.join(f'0x{byte:02X}' for byte in byte_list[i:i+8])
                    header.write(f'{line},\n')
                header.write('};\n')

                # Write the struct for the image in the source file
                source.write(f'const ImageBitmap {array_name} = {{\n')
                source.write(f'    {width}, {height}, {array_name}_bitmap\n')
                source.write('};\n\n')

                # Add extern declaration
                extern_declarations += f'extern const ImageBitmap {array_name};\n'

        header.write("\n" + extern_declarations)
        header.write("#endif /* SRC_LCD_BITMAP_H_ */\n")

# Usage
folder_path = 'screen_images'  # Input images folder
header_file = 'lcd_bitmap.h'   # Output header file
source_file = 'lcd_bitmap.c'   # Output source file
generate_header_file(folder_path, header_file, source_file)
