/*
   gpio.h

    Created on: Dec 12, 2018
        Author: Dan Walkes

    Updated by Dave Sluiter Sept 7, 2020. moved #defines from .c to .h file.
    Updated by Dave Sluiter Dec 31, 2020. Minor edits with #defines.

 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_
#include <stdbool.h>
#include "em_gpio.h"
#include <string.h>


// Student Edit: Define these, 0's are placeholder values.
// See the radio board user guide at https://www.silabs.com/documents/login/user-guides/ug279-brd4104a-user-guide.pdf
// and GPIO documentation at https://siliconlabs.github.io/Gecko_SDK_Doc/efm32g/html/group__GPIO.html
// to determine the correct values for these.

#define	TEST_LED_PORT  gpioPortC // change to correct ports and pins
#define TEST_LED_PIN   7

#define LCD_DISP_PORT gpioPortA
#define LCD_DISP_PIN 4

//switches
#define SW_PORT   gpioPortA
#define SW_SELECT_PIN 1
#define SW_DOWN_PIN 0
#define SW_UP_PIN 2

#define MAG_ENC_MUX_PORT gpioPortD
#define MAG_ENC_MUX_PIN 14

#define LASER_SW_PORT gpioPortA
#define LASER_SW_PIN 3
#define mag_enc_P_load_sw_port  gpioPortD
#define mag_enc_P_load_sw_pin   12
#define mag_enc_W_load_sw_port  gpioPortD
#define mag_enc_W_load_sw_pin   10
#define mag_enc_A_port  gpioPortD
#define mag_enc_A_pin   13
#define mag_enc_B_port  gpioPortD
#define mag_enc_B_pin   15


#define PB0_port gpioPortF
#define PB0_pin  6
#define PB1_port gpioPortF
#define PB1_pin  7


//Load switches
#define lcd_load_sw_port  gpioPortA
#define lcd_load_sw_pin   4

#define mag_enc_load_sw_port  gpioPortD
#define mag_enc_load_sw_pin   11


#define ultrasonic_load_sw_port gpioPortB
#define ultrasonic_load_sw_pin 11

#define extcom_port gpioPortB
#define extcom_pin 13



// Function prototypes
void gpioInit();
void gpioTestLedSetOff();
void gpioTestLedSetOn();
void gpioMagEncPSetOn();
void gpioMagEncPSetOff();
void gpioMagEncWSetOn();
void gpioMagEncWSetOff();
void gpioUltrasonicSetOn();
void gpioUltrasonicSetOff();
void gpioLcdSetOff();
void gpioLcdSetOn();
void gpioLaserSetOff();
void gpioLaserSetOn();
void enable_sensor();
void disable_sensor();
void gpioSetDisplayExtcomin(bool value);



#endif /* SRC_GPIO_H_ */
