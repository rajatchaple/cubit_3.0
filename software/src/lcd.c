#include "lcd.h"
#include "log.h"
#include "gpio.h"
#include "lcd_bitmap.h"
#include "sl_memlcd.h"
#include "sl_memlcd_display.h"

// Define the display size
#define DISPLAY_WIDTH 400
#define DISPLAY_HEIGHT 240

// Structure to hold display data
typedef struct {
    uint32_t dmdInitConfig;
    bool last_extcomin_state_high;
    GLIB_Context_t glibContext;
} display_data_t;

static display_data_t global_display_data;

static display_data_t* displayGetData() {
    return &global_display_data;
}

void displayInit() {
    EMSTATUS status;
    display_data_t *display = displayGetData();

    memset(display, 0, sizeof(display_data_t));
    display->last_extcomin_state_high = false;

    gpioLcdSetOn();

    status = DMD_init(0);
    if (status != DMD_OK) {
        LOG_ERROR("DMD_init() returned non-zero error code=0x%04x", (unsigned int)status);
    }

    status = GLIB_contextInit(&display->glibContext);
    if (status != GLIB_OK) {
        LOG_ERROR("GLIB_contextInit() returned non-zero error code=0x%04x", (unsigned int)status);
    }

    display->glibContext.backgroundColor = White;
    display->glibContext.foregroundColor = Black;

    status = GLIB_clear(&display->glibContext);
    if (status != GLIB_OK) {
        LOG_ERROR("GLIB_clear() returned non-zero error code=0x%04x", (unsigned int)status);
    }

    status = GLIB_setFont(&display->glibContext, (GLIB_Font_t *)&GLIB_FontNumber16x20);
    if (status != GLIB_OK) {
        LOG_ERROR("GLIB_setFont() returned non-zero error code=0x%04x", (unsigned int)status);
    }

    drawImage(&menu_frame, 0, 0);
    drawImage(&logo, 73-5, 400-91-94);
    drawImage(&logo_mini, 58-5, 400-334-31+4);


    status = DMD_updateDisplay();
    if (status != DMD_OK) {
        LOG_ERROR("DMD_updateDisplay() returned non-zero error code=0x%04x", (unsigned int)status);
    }

    sl_status_t timer_response = sl_bt_system_set_soft_timer(400, 2, 0); //32768 for 1 second
    if (timer_response != SL_STATUS_OK) {
        LOG_ERROR("Error in soft timer\n\r");
    }

    int a = 5500000;
    //workaround to automatically disappear welcome screen
    while(a != 0)
      {
a--;
      }
    handle_lcd_menus(evt_Button_UP);

//    gpioLcdSetOn();
}

void drawImage(const ImageBitmap* image, int X_off, int Y_off) {
    EMSTATUS status;
    display_data_t *display = displayGetData();
//
    status = DMD_setClippingArea(Y_off, X_off, image->height, image->width );
     if (status != DMD_OK) {
//       return status;
     }


       for (int r = 0; r < image->height; r++) {
               for (int c = 0; c < image->width; c++) {
                   int fake_flipped_r = image->height - 1 - r;
                   int display_r = /*Y_off + */fake_flipped_r;
                   int display_c =/* X_off +*/ c;
                   int index = image->width * r + c;

                   if (image->bitmap[index] == 0) {
                       status = GLIB_drawPixel(&display->glibContext, display_r, display_c);
                       if (status != GLIB_OK) {
                           LOG_ERROR("GLIB_drawPixel() returned non-zero error code=0x%04x", (unsigned int)status);
                       }
                   }
               }
           }

//       GLIB_applyClippingRegion(&display->glibContext);
       GLIB_resetDisplayClippingArea(&display->glibContext);

}

//void drawImage(const ImageBitmap* image, int X_off, int Y_off) {
//    EMSTATUS status;
//    display_data_t *display = displayGetData();
//
//    // Check for valid image pointer
//    if (image == NULL || image->bitmap == NULL) {
//        LOG_ERROR("Invalid image or bitmap pointer");
//        return;
//    }
//
//    for (int r = 0; r < image->height; r++) {
//        for (int c = 0; c < image->width; c++) {
//            int fake_flipped_r = image->height - 1 - r;
//            int display_r = Y_off + fake_flipped_r;
//            int display_c = X_off + c;
//            int index = image->width * r + c;
//
//            if (image->bitmap[index] == 0) {
//                status = GLIB_drawPixel(&display->glibContext, display_r, display_c);
//                if (status != GLIB_OK) {
//                    LOG_ERROR("GLIB_drawPixel() returned non-zero error code=0x%04x", (unsigned int)status);
//                }
//            }
//        }
//    }
//}
void clear_reading(){
  EMSTATUS status;
  display_data_t *display = displayGetData();

//    Because of rotated display, X becomes  y and Y becomes x. Follow commented api to fill in values
//    GLIB_Rectangle_t rect = { .yMin, .xMin, .yMax, .xMax};
  GLIB_Rectangle_t rect =    { 220,   20,    310,   220 };
  status = GLIB_setClippingRegion(&display->glibContext, &rect);
  if (status != GLIB_OK) {
      LOG_ERROR("GLIB_setClippingRegion() returned non-zero error code=0x%04x", (unsigned int)status);
  }

  status = GLIB_clearRegion(&display->glibContext);
  if (status != GLIB_OK) {
      LOG_ERROR("GLIB_clearRegion() returned non-zero error code=0x%04x", (unsigned int)status);
  }

  status = GLIB_resetClippingRegion(&display->glibContext);
  if (status != GLIB_OK) {
      LOG_ERROR("GLIB_resetClippingRegion() returned non-zero error code=0x%04x", (unsigned int)status);
  }
}




void displayUpdate() {
    display_data_t *display = displayGetData();
    EMSTATUS status;

//    display->last_extcomin_state_high = !display->last_extcomin_state_high;
//    gpioSetDisplayExtcomin(display->last_extcomin_state_high);

    status = DMD_updateDisplay();
    if (status != DMD_OK) {
        LOG_ERROR("DMD_updateDisplay() returned non-zero error code=0x%04x", (unsigned int)status);
    }
}

void clear_menu_arrow(){
  EMSTATUS status;
  display_data_t *display = displayGetData();

//    Because of rotated display, X becomes  y and Y becomes x. Follow commented api to fill in values
//    GLIB_Rectangle_t rect = { .yMin, .xMin, .yMax, .xMax};
  GLIB_Rectangle_t rect =    { 253,   192,    283,   232 };
  status = GLIB_setClippingRegion(&display->glibContext, &rect);
  if (status != GLIB_OK) {
      LOG_ERROR("GLIB_setClippingRegion() returned non-zero error code=0x%04x", (unsigned int)status);
  }

  status = GLIB_clearRegion(&display->glibContext);
  if (status != GLIB_OK) {
      LOG_ERROR("GLIB_clearRegion() returned non-zero error code=0x%04x", (unsigned int)status);
  }

  status = GLIB_resetClippingRegion(&display->glibContext);
  if (status != GLIB_OK) {
      LOG_ERROR("GLIB_resetClippingRegion() returned non-zero error code=0x%04x", (unsigned int)status);
  }
}

//void clearImageRegion(int yMin, int xMin, int yMax, int xMax) {
void clear_image(const ImageBitmap* image, int X_off, int Y_off){
    EMSTATUS status;
    display_data_t *display = displayGetData();

//    Because of rotated display, X becomes  y and Y becomes x. Follow commented api to fill in values
//    GLIB_Rectangle_t rect = { .yMin, .xMin, .yMax, .xMax};
    GLIB_Rectangle_t rect =    { Y_off,   X_off,    Y_off + image->height,   X_off + image->width};
    status = GLIB_setClippingRegion(&display->glibContext, &rect);
    if (status != GLIB_OK) {
        LOG_ERROR("GLIB_setClippingRegion() returned non-zero error code=0x%04x", (unsigned int)status);
    }

    status = GLIB_clearRegion(&display->glibContext);
    if (status != GLIB_OK) {
        LOG_ERROR("GLIB_clearRegion() returned non-zero error code=0x%04x", (unsigned int)status);
    }

    status = GLIB_resetClippingRegion(&display->glibContext);
    if (status != GLIB_OK) {
        LOG_ERROR("GLIB_resetClippingRegion() returned non-zero error code=0x%04x", (unsigned int)status);
    }
}

//void clearImageRegion(int yMin, int xMin, int yMax, int xMax) {
void clear_menu_top_display(){
    EMSTATUS status;
    display_data_t *display = displayGetData();

//    Because of rotated display, X becomes  y and Y becomes x. Follow commented api to fill in values
//    GLIB_Rectangle_t rect = { .yMin, .xMin, .yMax, .xMax};
    GLIB_Rectangle_t rect =    { 170,   39-5,    362,   205-5};
    status = GLIB_setClippingRegion(&display->glibContext, &rect);
    if (status != GLIB_OK) {
        LOG_ERROR("GLIB_setClippingRegion() returned non-zero error code=0x%04x", (unsigned int)status);
    }

    status = GLIB_clearRegion(&display->glibContext);
    if (status != GLIB_OK) {
        LOG_ERROR("GLIB_clearRegion() returned non-zero error code=0x%04x", (unsigned int)status);
    }

    status = GLIB_resetClippingRegion(&display->glibContext);
    if (status != GLIB_OK) {
        LOG_ERROR("GLIB_resetClippingRegion() returned non-zero error code=0x%04x", (unsigned int)status);
    }

}


void display_reading(int number, bool large_digits_only) {
    clear_reading();
//    int positions[5][2] = {{9+45, 400-11-80-87}, {61+45, 400-11-80-87}, {115+45, 400-53-36-87}, {138+45, 400-53-36-87}, {198, 224}};
    int positions[5][2] = {{9+45, 400-5-80-87}, {61+45, 400-5-80-87}, {115+45, 400-47-36-87}, {138+45, 400-47-36-87}, {198, 400-47-36-8}};
    char str[6];
    snprintf(str, 6, "%05d", number); // Convert number to zero-padded 5 digit string

    if (large_digits_only) {
        if(number >= 1000 || number == 647 || number == 255)
          {

          }
        else if(number >= 100)
          {
            // Display only the two most significant digits as large digits
              int hundredth_place = (number / 100) % 10; // Second most significant digit
              int tenth_place = (number / 10) % 10; // Second most significant digit
              int unit_place = number % 10; // Most significant digit
              positions[0][0] -= 5;
              positions[1][0] -= 5;
              positions[2][0] -= 5;

              drawImage(&large_digits[hundredth_place], positions[0][0]-5, positions[0][1]);
              drawImage(&large_digits[tenth_place], positions[1][0]-5, positions[1][1]);
              drawImage(&large_digits[unit_place], positions[2][0]-5, positions[2][1]);
          }
        else if(number < 100)
          {
        // Display only the two most significant digits as large digits
        int digit1 = (number / 10) % 10; // Second most significant digit
        int digit2 = number % 10; // Most significant digit
        positions[0][0] += 20;
        positions[1][0] += 20;

        drawImage(&large_digits[digit1], positions[0][0]-5, positions[0][1]);
        drawImage(&large_digits[digit2], positions[1][0]-5, positions[1][1]);
          }
    } else {
        if (number >= 100000) {
            // If number is greater than or equal to 100000, display "88888"
            for (int i = 0; i < 5; i++) {
                if (i < 3) {
                    drawImage(&large_digits[8], positions[i][0]-5, positions[i][1]);
                } else {
                    drawImage(&small_digits[8], positions[i][0]-5, positions[i][1]);
                }
            }
        } else if (number >= 10000) {
            // Adjust positions for 5 digits
            positions[0][0] -= 30;
            positions[1][0] -= 30;
            positions[2][0] -= 35;
            positions[3][0] -= 10;

            // Display the digits
            for (int i = 0; i < 5; i++) {
                int digit = str[i] - '0';
                if (i < 3) {
                    drawImage(&large_digits[digit], positions[i][0]-5, positions[i][1]);
                } else {
                    drawImage(&small_digits[digit], positions[i][0]-5, positions[i][1]);
                }
            }
        } else {
            // Display up to 4 digits
            for (int i = 0; i < 4; i++) {
                int digit = str[i + 1] - '0';
                if (i < 2) {
                    drawImage(&large_digits[digit], positions[i][0]-5, positions[i][1]);
                } else {
                    drawImage(&small_digits[digit], positions[i][0]-5, positions[i][1]);
                }
            }
        }
    }

    displayUpdate();
}

