import cv2
import glob
import numpy as np
import os

dirname = os.path.dirname(__file__)
screens_folder = os.path.join(dirname, 'screens/LCD_screeens_NewAlgo')
  

class image_class:
    def __init__(self, img_name):
        self.img = cv2.imread(img_name)
        self.__name = img_name

    def __str__(self):
        return self.__name

img_list = []

def read_img(img_list, img):
    n = cv2.imread(img, 2)
    img_list.append(n)
    return img_list

img_names_list = []

for img_file in os.listdir(screens_folder):
  img_names_list.append(img_file)
  img_file_path = screens_folder + "/" + img_file
  # img_object = image_class(img_file_path)
  read_img(img_list, img_file_path)
  # print(str(img_object))
  # print(img_object.img)


print(img_names_list)






# path = glob.glob(screens_folder + "/*.PNG") #or jpg
# print(path)
# list_ = []

# cv_images = [read_img(list_, image) for image in path]
img_index = 0


for i,images in enumerate(img_list,0):
  
  # for i,img in enumerate(images,0):
  # read the image file
  # print(type(img))
  # cv2.imshow("Original", img)
  img = cv2.resize(images, (168,144), interpolation = cv2.INTER_AREA)
  # cv2.imshow("Resized", img)
  # ret, bw_img = cv2.threshold(img, 127, 255, cv2.THRESH_BINARY)
    
  # converting to its binary form
  bw = cv2.threshold(img, 127, 255, cv2.THRESH_BINARY)
  bw[1][bw[1] > 127] = 1

  # print(bw[1])
  # one_d_array = (bw[1].ravel())
  # print(bw[1].tolist())
  # print(one_d_array)
  # cv2.imshow("Binary", bw_img)
  # one_d_array = np.transpose(one_d_array)
  # for img_row in bw[1]:
  # print(img_row)
  with open("bitmap_raw.h", 'a') as file:
    file.write("static const char ")
    file.write(img_names_list[i][:-4])
    file.write("[] = {\n")
    np.savetxt(file, bw[1], fmt='%d', delimiter=',')   # X is an array
    file.write("}\n")
      
  # np.savetxt('bitmap_raw.h', bw[1], fmt='%d', delimiter=',')   # X is an array


appendText=','
names=open("bitmap_raw.h",'r')
updatedNames=open("lcd_bitmap.h",'a')
for name in names:
  if not '{' in name:
    updatedNames.write(name.rstrip() + appendText + '\n')
  else:
    updatedNames.write(name.rstrip() + '\n')
updatedNames.close()


# replacing ,\n] with ]\n
# Read in the file
with open('lcd_bitmap.h', 'r') as file :
  filedata = file.read()

# Replace the target string
filedata = filedata.replace(",\n},", "};\n")

# Write the file out again
with open('lcd_bitmap.h', 'w') as file:
  file.write("/* \n* lcd_bitmap.h\n *\n *  Created on: Feb 18, 2022\n *      Author: rajat\n */\n\n#ifndef SRC_LCD_BITMAP_H_\n#define SRC_LCD_BITMAP_H_\n\n")
  file.write(filedata)
  file.write("#endif /* SRC_LCD_BITMAP_H_ */")

cv2.waitKey(0)
cv2.destroyAllWindows()