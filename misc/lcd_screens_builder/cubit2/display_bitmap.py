import re
import numpy as np
import matplotlib.pyplot as plt

def parse_bitmap_data(header_file, source_file):
    with open(header_file, 'r') as file:
        header_content = file.read()
    
    with open(source_file, 'r') as file:
        source_content = file.read()
    
    # Regular expression to find all bitmaps in the header file
    bitmap_pattern = re.compile(r'static const char (\w+)_bitmap\[\] = \{([^}]*)\};', re.MULTILINE | re.DOTALL)
    bitmaps = bitmap_pattern.findall(header_content)
    
    # Regular expression to find width and height in the source file
    size_pattern = re.compile(r'const ImageBitmap (\w+) = \{\s*(\d+),\s*(\d+),\s*\1_bitmap\s*\};', re.MULTILINE | re.DOTALL)
    sizes = size_pattern.findall(source_content)
    
    return bitmaps, {name: (int(width), int(height)) for name, width, height in sizes}

def display_bitmap(bitmaps, sizes):
    for name, data_str in bitmaps:
        data_str = data_str.replace('\n', '').replace(' ', '')
        data_bytes = [int(byte, 16) for byte in data_str.split(',') if byte]
        
        if name not in sizes:
            print(f"Size information for {name} not found.")
            continue
        
        width, height = sizes[name]
        binary_data = ''.join(f'{byte:08b}' for byte in data_bytes)
        image_data = np.array([int(bit) for bit in binary_data[:width*height]]).reshape((height, width))
        
        plt.figure(figsize=(8, 8))
        plt.title(name)
        plt.imshow(image_data, cmap='gray')
        plt.axis('off')
        plt.show()

# Usage
header_file = 'C:\\Users\\rajat\\OneDrive\\Low Power\\cubit2.0\\lpedt_cubit_2.0\\misc\\lcd_screens_builder\\cubit2\\lcd_bitmap.h'
source_file = 'C:\\Users\\rajat\\OneDrive\\Low Power\\cubit2.0\\lpedt_cubit_2.0\\misc\\lcd_screens_builder\\cubit2\\lcd_bitmap.c'
bitmaps, sizes = parse_bitmap_data(header_file, source_file)
display_bitmap(bitmaps, sizes)
