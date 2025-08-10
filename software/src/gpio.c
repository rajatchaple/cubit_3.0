/*
  gpio.c

   Created on: Dec 12, 2018
       Author: Dan Walkes
   Updated by Dave Sluiter Dec 31, 2020. Minor edits with #defines.

   March 17
   Dave Sluiter: Use this file to define functions that set up or control GPIOs.

 */




#include "gpio.h"
#include "../app.h"
#include "main.h"





// Set GPIO drive strengths and modes of operation
void gpioInit()
{


//  //GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
//  GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, false);


//  GPIO_DriveStrengthSet(sensor_port, gpioDriveStrengthWeakAlternateWeak);
//  GPIO_PinModeSet(sensor_port, sensor_pin, gpioModePushPull, false);

  GPIO_DriveStrengthSet(lcd_load_sw_port, gpioDriveStrengthWeakAlternateWeak);
  GPIO_PinModeSet(lcd_load_sw_port, lcd_load_sw_pin, gpioModePushPull, false);

  GPIO_DriveStrengthSet(mag_enc_P_load_sw_port, gpioDriveStrengthWeakAlternateWeak);
  GPIO_PinModeSet(mag_enc_P_load_sw_port, mag_enc_P_load_sw_pin, gpioModePushPull, false);

  GPIO_DriveStrengthSet(mag_enc_W_load_sw_port, gpioDriveStrengthWeakAlternateWeak);
  GPIO_PinModeSet(mag_enc_W_load_sw_port, mag_enc_W_load_sw_pin, gpioModePushPull, false);

  GPIO_DriveStrengthSet(MAG_ENC_MUX_PORT, gpioDriveStrengthWeakAlternateWeak);
  GPIO_PinModeSet(MAG_ENC_MUX_PORT, MAG_ENC_MUX_PIN, gpioModePushPull, false);

  GPIO_DriveStrengthSet(ultrasonic_load_sw_port, gpioDriveStrengthWeakAlternateWeak);
  GPIO_PinModeSet(ultrasonic_load_sw_port, ultrasonic_load_sw_pin, gpioModePushPull, false);

  GPIO_DriveStrengthSet(LASER_SW_PORT, gpioDriveStrengthWeakAlternateWeak);
  GPIO_PinModeSet(LASER_SW_PORT, LASER_SW_PIN, gpioModePushPull, false);

  GPIO_DriveStrengthSet(TEST_LED_PORT, gpioDriveStrengthWeakAlternateWeak);
  GPIO_PinModeSet(TEST_LED_PORT, TEST_LED_PIN, gpioModePushPull, false);


//  GPIO_DriveStrengthSet(extcom_port, gpioDriveStrengthWeakAlternateWeak);
//  GPIO_PinModeSet(extcom_port, extcom_pin, gpioModePushPull, false);

    GPIO_DriveStrengthSet(LCD_DISP_PORT, gpioDriveStrengthWeakAlternateWeak);
    GPIO_PinModeSet(LCD_DISP_PIN, LCD_DISP_PIN, gpioModePushPull, false);

//  GPIO_PinModeSet(PB0_port, PB0_pin, gpioModeInputPullFilter, true);
//  GPIO_ExtIntConfig(PB0_port, PB0_pin, PB0_pin, true, true, true);

//  GPIO_PinModeSet(PB1_port, PB1_pin, gpioModeInputPullFilter, true);
//  GPIO_ExtIntConfig(PB1_port, PB1_pin, PB1_pin, true, true, true);

//  GPIO_PinModeSet(IMU_INTRPT_port, IMU_INTRPT_pin, gpioModeInputPullFilter, true);  //DOUT : true means pull up
//  GPIO_ExtIntConfig (IMU_INTRPT_port, IMU_INTRPT_pin, IMU_INTRPT_pin, false, true, true);  //enable at falling edge



  //HMI switches pin configuration
  GPIO_PinModeSet(SW_PORT, SW_SELECT_PIN, gpioModeInputPullFilter, true);
  GPIO_ExtIntConfig(SW_PORT, SW_SELECT_PIN, SW_SELECT_PIN, true, false, true);

  GPIO_PinModeSet(SW_PORT, SW_DOWN_PIN, gpioModeInputPullFilter, true);
  GPIO_ExtIntConfig(SW_PORT, SW_DOWN_PIN, SW_DOWN_PIN, true, false, true);

  GPIO_PinModeSet(SW_PORT, SW_UP_PIN, gpioModeInputPullFilter, true);
  GPIO_ExtIntConfig(SW_PORT, SW_UP_PIN, SW_UP_PIN, true, false, true);

  GPIO_IntClear(0xFFFFFFFF);
} // gpioInit()

void gpioTestLedSetOn()
{
  GPIO_PinOutSet(TEST_LED_PORT,TEST_LED_PIN);
}


void gpioTestLedSetOff()
{
  GPIO_PinOutClear(TEST_LED_PORT,TEST_LED_PIN);
}

void gpioUsePulleyMagEnc()
{
  GPIO_PinOutClear(MAG_ENC_MUX_PORT,MAG_ENC_MUX_PIN);
}

void gpioUseWheelMagEnc()
{
  GPIO_PinOutSet(MAG_ENC_MUX_PORT,MAG_ENC_MUX_PIN);
}



void gpioMagEncPSetOff()
{
  GPIO_PinOutSet(mag_enc_P_load_sw_port,mag_enc_P_load_sw_pin);

  //turning GPIOs off to save on power
//  GPIO_DriveStrengthSet(mag_enc_A_port, gpioDriveStrengthWeakAlternateWeak);
  GPIO_PinModeSet(mag_enc_A_port, mag_enc_A_pin, gpioModeDisabled, false);
//  GPIO_PinOutSet(mag_enc_A_port,mag_enc_A_pin);

//  GPIO_DriveStrengthSet(mag_enc_B_port, gpioDriveStrengthWeakAlternateWeak);
  GPIO_PinModeSet(mag_enc_B_port, mag_enc_B_pin, gpioModeDisabled, false);
//  GPIO_PinOutSet(mag_enc_B_port,mag_enc_B_pin);
}


void gpioMagEncPSetOn()
{
  GPIO_PinOutClear(mag_enc_P_load_sw_port,mag_enc_P_load_sw_pin);

  gpioUsePulleyMagEnc();

  GPIO_PinModeSet(mag_enc_A_port, mag_enc_A_pin, gpioModeInputPullFilter, false);
  GPIO_ExtIntConfig(mag_enc_A_port, mag_enc_A_pin, mag_enc_A_pin, true, true, true);

  GPIO_PinModeSet(mag_enc_B_port, mag_enc_B_pin, gpioModeInputPullFilter, false);
  GPIO_ExtIntConfig(mag_enc_B_port, mag_enc_B_pin, mag_enc_B_pin, true, true, true);
}

void gpioMagEncWSetOff()
{
  GPIO_PinOutSet(mag_enc_W_load_sw_port,mag_enc_W_load_sw_pin);

  //turning GPIOs off to save on power
//  GPIO_DriveStrengthSet(mag_enc_A_port, gpioDriveStrengthWeakAlternateWeak);
  GPIO_PinModeSet(mag_enc_A_port, mag_enc_A_pin, gpioModeDisabled, false);
//  GPIO_PinOutSet(mag_enc_A_port,mag_enc_A_pin);

//  GPIO_DriveStrengthSet(mag_enc_B_port, gpioDriveStrengthWeakAlternateWeak);
  GPIO_PinModeSet(mag_enc_B_port, mag_enc_B_pin, gpioModeDisabled, false);
//  GPIO_PinOutSet(mag_enc_B_port,mag_enc_B_pin);
}


void gpioMagEncWSetOn()
{
  GPIO_PinOutClear(mag_enc_W_load_sw_port,mag_enc_W_load_sw_pin);
  gpioUseWheelMagEnc();

  GPIO_PinModeSet(mag_enc_A_port, mag_enc_A_pin, gpioModeInputPull, true);
  GPIO_ExtIntConfig(mag_enc_A_port, mag_enc_A_pin, mag_enc_A_pin, true, true, true);

  GPIO_PinModeSet(mag_enc_B_port, mag_enc_B_pin, gpioModeInputPull, true);
  GPIO_ExtIntConfig(mag_enc_B_port, mag_enc_B_pin, mag_enc_B_pin, true, true, true);
}



void gpioUltrasonicSetOff()
{
  GPIO_PinOutSet(ultrasonic_load_sw_port,ultrasonic_load_sw_pin);
  gpioLaserSetOff();
}


void gpioUltrasonicSetOn()
{
  GPIO_PinOutClear(ultrasonic_load_sw_port,ultrasonic_load_sw_pin);
  gpioLaserSetOn();
}


void gpioLcdSetOn()
{
  GPIO_PinOutSet(LCD_DISP_PORT,LCD_DISP_PIN);
}


void gpioLcdSetOff()
{
  GPIO_PinOutClear(LCD_DISP_PORT,LCD_DISP_PIN);
}


void gpioLaserSetOff()
{
  GPIO_PinOutSet(LASER_SW_PORT,LASER_SW_PIN);


}


void gpioLaserSetOn()
{
  GPIO_PinOutClear(LASER_SW_PORT,LASER_SW_PIN);
}






void gpioSetDisplayExtcomin(bool value) {

//  GPIO_PinOutClear(extcom_port, extcom_pin);
//  if(value == true) {
//      GPIO_PinOutSet(lcd_port, lcd_pin);
//  }
//  else {
//      GPIO_PinOutClear(lcd_port, lcd_pin);
//  }
}




