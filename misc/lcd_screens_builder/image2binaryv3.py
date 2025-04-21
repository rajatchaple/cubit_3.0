import os
from PIL import Image
import numpy as np
from datetime import datetime

def create_bitmap(image_path):
    image = Image.open(image_path).convert('1')  # Convert to 1-bit monochrome
    image_array = np.array(image, dtype=np.uint8)
    # Convert image array to bitmap (1 for white, 0 for black)
    bitmap_array = np.where(image_array == 0, 1, 0)
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

    os.makedirs(os.path.dirname(header_file), exist_ok=True)
    os.makedirs(os.path.dirname(source_file), exist_ok=True)

    with open(header_file, 'w') as header, open(source_file, 'w') as source:
        header.write(header_content)
        source.write(source_content)

        for filename in os.listdir(folder_path):
            if filename.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp')):
                image_path = os.path.join(folder_path, filename)
                bitmap_array, (height, width) = create_bitmap(image_path)
                array_name = os.path.splitext(filename)[0]

                # Convert the bitmap array to the required format
                byte_list = []
                for row in bitmap_array:
                    bits = ''.join(map(str, row))
                    # Pad bits if necessary to make them a multiple of 8
                    bits = bits.ljust((len(bits) + 7) // 8 * 8, '0')
                    byte_list.extend([int(bits[i:i+8], 2) for i in range(0, len(bits), 8)])

                # Write the bitmap array in the header file
                header.write(f'static const char {array_name}_bitmap[] = {{\n')
                for i in range(0, len(byte_list), 12):  # Group by 12 bytes per line for readability
                    line = ', '.join(f'0x{byte:02X}' for byte in byte_list[i:i+12])
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
folder_path = 'C:\\Users\\rajat\\OneDrive\\Low Power\\cubit2.0\\lpedt_cubit_2.0\\misc\\lcd_screens_builder\\cubit2\\screen_images'
header_file = 'C:\\Users\\rajat\\OneDrive\\Low Power\\cubit2.0\\lpedt_cubit_2.0\\misc\\lcd_screens_builder\\cubit2\\lcd_bitmap.h'
source_file = 'C:\\Users\\rajat\\OneDrive\\Low Power\\cubit2.0\\lpedt_cubit_2.0\\misc\\lcd_screens_builder\\cubit2\\lcd_bitmap.c'
generate_header_file(folder_path, header_file, source_file)

# For debugging: Print the bitmap data
for filename in os.listdir(folder_path):
    if filename.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp')):
        image_path = os.path.join(folder_path, filename)
        bitmap_array, (height, width) = create_bitmap(image_path)
        array_name = os.path.splitext(filename)[0]
        print(f'{array_name}_bitmap (size: {width}x{height}):')
        for row in bitmap_array:
            print(''.join(str(bit) for bit in row))
