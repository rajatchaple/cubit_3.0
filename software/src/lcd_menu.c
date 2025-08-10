/*
 * lcd_menu.c
 *
 *  Created on: Mar 31, 2022
 *      Author: rajat
 */

#include "src/lcd_menu.h"
#include "src/imu.h"
#include "stdint.h"
#include <stdio.h>
#include "scheduler.h"
#include "lcd_bitmap.h"
#include "ultrasonic.h"
#include "ble.h"
#include "lcd.h"

extern mag_enc_t mag_enc;
uint32_t curr_lcd_screen = 0;

#include <stdint.h>

typedef enum {
    MENU_PULLEY,
    MENU_SONIC,
    MENU_WHEEL,
    MENU_ANGLE,
    MENU_SETTINGS,
    MENU_COUNT
} MenuItems;

typedef enum {
    LCD_HOME_SCREEN,
    LCD_READING_SCREEN,
    LCD_SETTINGS_SCREEN,
    LCD_FLASH_SCREEN,
    LCD_ABOUT_SCREEN
} ScreenType;

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    MenuItems item;
    ScreenType nextScreen;
    ImageBitmap image;
    Position homePosition;
} MenuItem;

typedef struct {
    MenuItem items[MENU_COUNT];
    int currentIndex;
} Menu;

typedef struct {
    ScreenType currentScreenType;
    Menu menu;
    Position readingLayout;  // Simplified for example, replace with actual layout if needed
} StateMachine;

typedef struct {
    int x;
    int y;
} Dot;

StateMachine stateMachine;
Dot dot_position;

void init_lcd_menu() {
    stateMachine.currentScreenType = LCD_HOME_SCREEN;
    stateMachine.menu.items[0] = (MenuItem){MENU_WHEEL, LCD_READING_SCREEN, wheel_icon, {130, 330}};
    stateMachine.menu.items[1] = (MenuItem){MENU_SONIC, LCD_READING_SCREEN, sonic_icon, {148, 297}};
    stateMachine.menu.items[2] = (MenuItem){MENU_PULLEY, LCD_READING_SCREEN, pulley_icon, {152, 258}};
    stateMachine.menu.items[3] = (MenuItem){MENU_ANGLE, LCD_READING_SCREEN, angle_icon, {147, 214}};
    stateMachine.menu.items[4] = (MenuItem){MENU_SETTINGS, LCD_SETTINGS_SCREEN, settings_icon, {130, 172}};
    stateMachine.menu.currentIndex = 0;
    stateMachine.readingLayout = (Position){100, 50};  // Example values for the reading screen layout
    gpioLcdSetOn();
}

void update_home_screen(const StateMachine *sm, int rotate_counter)
{
//  drawImage(&menu_arrow, 192, (400-147));
  clear_menu_top_display();
  clear_image(&settings_name, 29-5, 400 - 24 - 334);
  if(rotate_counter == 1)
    drawImage(&arc1, 100-5, 178);
  else if(rotate_counter == 2)
    drawImage(&arc2, 100-5, 178);
  else if(rotate_counter == 3)
        drawImage(&arc3, 100-5, 178);

//    if (sm->currentScreenType == LCD_HOME_SCREEN)
      {

        for (int i = 0; i < MENU_COUNT; i++) {
            drawImage(&(sm->menu.items[i].image), sm->menu.items[i].homePosition.x-5, sm->menu.items[i].homePosition.y);
        }
    }

      drawImage(&dot, dot_position.x-5, dot_position.y);

      // Determine which large icon to draw based on the current menu index
          switch(sm->menu.items[sm->menu.currentIndex].item) {
              case MENU_WHEEL:
                  drawImage(&large_wheel_icon, 39-5, 400 - 93 - 70);
                  drawImage(&wheel_name, 29-5, 400 - 24 - 334+4);
                  break;
              case MENU_SONIC:
                  drawImage(&large_sonic_icon, 39-5, 400 - 93 - 70);
                  drawImage(&sonic_name, 29-5, 400 - 24 - 334+4);
                  break;
              case MENU_PULLEY:
                  drawImage(&large_pulley_icon, 39-5, 400 - 93 - 70);
                  drawImage(&pulley_name, 29-5, 400 - 24 - 334+4);
                  break;
              case MENU_ANGLE:
                  drawImage(&large_angle_icon, 39-5, 400 - 93 - 70);
                  drawImage(&angle_name, 29-5, 400 - 24 - 334+4);
                  break;
              case MENU_SETTINGS:
                  drawImage(&large_settings_icon, 39-5, 400 - 93 - 70);
                  drawImage(&settings_name, 29-5, 400 - 24 - 334+4);
                  break;
              default:
                  // Handle any unexpected cases
                  break;
          }

    // Additional screen types can be handled here

//    displayUpdate();
}

void update_dot_position(int *rotate_counter) {
    // Move the dot next to the current menu icon
    MenuItem current_item = stateMachine.menu.items[stateMachine.menu.currentIndex];
    dot_position.x = current_item.homePosition.x + 40; // Adjust the offset as needed
    dot_position.y = current_item.homePosition.y + 8;
    update_home_screen(&stateMachine, *rotate_counter);

}

void move_anticlockwise(int *rotate_counter) {
    // Rotate elements except positions
    stateMachine.menu.currentIndex = (stateMachine.menu.currentIndex + 1) % MENU_COUNT;

    *rotate_counter -= 1;
    if(*rotate_counter == 0)
        *rotate_counter = 3;

    update_dot_position(rotate_counter);
//    update_home_screen(&stateMachine, *rotate_counter);
}

void move_clockwise(int *rotate_counter) {
  // Rotate elements except positions
  stateMachine.menu.currentIndex = (stateMachine.menu.currentIndex - 1 + MENU_COUNT) % MENU_COUNT;

    *rotate_counter += 1;
    if(*rotate_counter == 4)
        *rotate_counter = 1;
    update_dot_position(rotate_counter);
//    update_home_screen(&stateMachine, *rotate_counter);
}

ImageBitmap* measurement_unit = &cm;

void handle_peripherals_for_readings_menu(uint32_t screen_type)
{
  sensor_data_t* sensor_data =  get_sensor_data();
  if(screen_type == LCD_READING_SCREEN)
    {
      switch(stateMachine.menu.items[stateMachine.menu.currentIndex].item)
      {
        case MENU_WHEEL:
                  {
//                    sensor_data->refreshing_mag_enc_reading = 0;
                    gpioMagEncWSetOn();
                    sensor_data->is_mag_enc_string = false;
                    sensor_data->is_mag_enc_enabled = true;
                    if(sensor_data->settings_unit == CM)
                     {
                         measurement_unit = &cm;
                     }
                     else if(sensor_data->settings_unit == INCH)
                     {
                         measurement_unit = &inch;
                     }
//                    sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
                          //set flag that would keep updating measured values using soft_time
                  }
                  break;
        case MENU_SONIC:
                  {
                    gpioUltrasonicSetOn();
                    sensor_data->is_ultrasonic_enabled = true;
                    if(sensor_data->settings_unit == CM)
                     {
                         measurement_unit = &cm;
                     }
                     else if(sensor_data->settings_unit == INCH)
                     {
                         measurement_unit = &inch;
                     }
                    //if unit is set as cm
//                    if(sensor_data->settings_unit==CM)
//                        displayUnit(DISPLAY_ROW_BTADDR, "cm");
//                    else if(sensor_data->settings_unit==INCH)
//                        displayUnit(DISPLAY_ROW_BTADDR, "inch");
                  }
                  break;

        case MENU_PULLEY:
                  {
//                    sensor_data->refreshing_mag_enc_reading = 0;
                    gpioMagEncPSetOn();
                    sensor_data->is_mag_enc_string = true;
                    sensor_data->is_mag_enc_enabled = true;
                    if(sensor_data->settings_unit == CM)
                     {
                         measurement_unit = &cm;
                     }
                     else if(sensor_data->settings_unit == INCH)
                     {
                         measurement_unit = &inch;
                     }
                  }
                  break;
        case MENU_ANGLE:
                  {
                    measurement_unit = &deg;
                    sensor_data->is_angular_enabled = true;
                    imu_init_state_machine(evt_NoEvent);
                  }
                  break;
        case MENU_COUNT:
                    {

                    }
                    break;
        }
    }
  else
    {
      /*disabling imu*/
      sensor_data->is_angular_enabled = false;
      sensor_data->is_imu_initialization_complete = false;
      sensor_data->is_imu_reading_ready = false;
      imu_turnoff_state_machine(evt_NoEvent);

      /*disabling ultrasonic sensor*/
      gpioUltrasonicSetOff();
      sensor_data->is_ultrasonic_enabled = false;


      gpioMagEncWSetOff();
      sensor_data->is_mag_enc_enabled = false;

      gpioMagEncPSetOff();
      sensor_data->is_mag_enc_enabled = false;  // move this to gpioMagEncSetOff()
    }

}

void handle_lcd_menus(uint32_t event) {
    static uint32_t next_lcd_screen = LCD_FLASH_SCREEN;
    static uint32_t current_lcd_screen = 0;
    static int rotate_counter = 1;
    sensor_data_t* sensor_data =  get_sensor_data();
    static bool back_enabled = false;
    static uint8_t dot_position = 0;
    static int cursor_pos = 0;

//    if (next_lcd_screen != current_lcd_screen) {
//        stateMachine.currentScreenType = LCD_HOME_SCREEN;
//        update_home_screen(&stateMachine, rotate_counter);
//        current_lcd_screen = next_lcd_screen;
//    }
    current_lcd_screen = next_lcd_screen;

    switch (current_lcd_screen) {
        case LCD_FLASH_SCREEN:
            if ((event == evt_Button_DOWN) || (event == evt_Button_UP) || (event == evt_Button_SELECT)) {
                clear_image(&logo, 73-5, 400-91-94);
                clear_image(&logo_mini, 58-5, 400-334-31+4);
                drawImage(&bluetooth_icon, 156-5, 400-333-27+4);
                drawImage(&battery_icon, 185-5, 400-334-25+4);
                update_dot_position(&rotate_counter);

                next_lcd_screen = LCD_HOME_SCREEN;
                sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM2);
                displayUpdate();
            }
            break;

        case LCD_HOME_SCREEN:
            stateMachine.currentScreenType = LCD_HOME_SCREEN;

            if (event == evt_Button_DOWN) {
                move_clockwise(&rotate_counter);
                displayUpdate();

            } else if (event == evt_Button_UP) {
                move_anticlockwise(&rotate_counter);
                displayUpdate();

            } else if (event == evt_Button_SELECT) {
                if(stateMachine.menu.currentIndex == (MENU_COUNT-1))
                  {
                    next_lcd_screen = LCD_SETTINGS_SCREEN;
                    clear_menu_top_display();
                    clear_image(&settings_name, 29-5, 400 - 24 - 334+4);

                    drawImage(&back_icon, 28-5, 400-334-25+4);
                    //drawImage(&change_unit, 28, 400-93-38);
                    drawImage(&about, 38-5, 400-34-56+4);
                    drawImage(&unit, 38-5, 400-96-33+4);
//                    drawImage(&dot, 194, 400-107-15 + 8);
                    drawImage(&dot, 194-5, 400-78-15 + 8);



                  }
                else
                  {
                      next_lcd_screen = LCD_READING_SCREEN;
                      clear_menu_top_display();
                      clear_image(&settings_name, 29-5, 400 - 24 - 334+4);
                      sensor_data->refreshing_mag_enc_reading = 0;
                      drawImage(&back_icon, 28-5, 400-334-25+4);
                      drawImage(&stateMachine.menu.items[stateMachine.menu.currentIndex].image, 28 + 38-5, 400-332-28+4);
      //                display_reading(2345, false);
      //                drawImage(&l1, 40, 400-82-99);
                      handle_peripherals_for_readings_menu(LCD_READING_SCREEN);  //entering LCD_READING_SCREEN

                      if(sensor_data->settings_unit == CM)
                        {
                          drawImage(measurement_unit, 101-5, 400-196-29);
                        }
                      else if(sensor_data->settings_unit == INCH)
                        {
                          drawImage(measurement_unit, 95-5, 400-196-29);
                        }
                      else if(sensor_data->settings_unit == DEG)
                        {
                          drawImage(measurement_unit, 100-5, 400-200-29);
                        }
                  }
//                update_home_screen(&stateMachine, rotate_counter);
                displayUpdate();
            }
            break;

        case LCD_READING_SCREEN:
            stateMachine.currentScreenType = LCD_READING_SCREEN;
            if (event == evt_Button_SELECT && back_enabled == true) {
                clear_image(&back_icon_selected, 28-5, 400-332-25+4);
                clear_image(&back_icon, 28-5, 400-332-27+4);
                next_lcd_screen = LCD_HOME_SCREEN;
                handle_peripherals_for_readings_menu(LCD_HOME_SCREEN);
                clear_image(&stateMachine.menu.items[stateMachine.menu.currentIndex].image, 28 + 38-5, 400-332-28);
                back_enabled = false;

//                Clearing unit
                clear_image(&inch, 95-5, 400-200-29); //inch being the widest, any unit with this would be cleared
                update_home_screen(&stateMachine, rotate_counter);
                displayUpdate();
            }
            if (event == evt_Button_UP || event == evt_Button_DOWN)
              {
                  next_lcd_screen = LCD_READING_SCREEN;
                  if(back_enabled == false)
                    {
                      clear_image(&back_icon, 28-5, 400-332-29+4);
                      drawImage(&back_icon_selected, 28-5, 400-334-25+4);
                      back_enabled = true;
                    }
                  else
                    {
                      clear_image(&back_icon_selected, 28-5, 400-332-27+4);
                      drawImage(&back_icon, 28-5, 400-332-27+4);
                      back_enabled = false;
                    }

//                  update_home_screen(&stateMachine, rotate_counter);
                  displayUpdate();
              }
            break;

        case LCD_SETTINGS_SCREEN:
            stateMachine.currentScreenType = LCD_SETTINGS_SCREEN;
            if(event == evt_Button_SELECT && cursor_pos == 0)
              {
                //400,196,30   64
                clear_menu_top_display();

                clear_image(&bluetooth_icon, 156-5, 400-333-27+4);
                clear_image(&battery_icon, 185-5, 400-334-25+4);
                clear_image(&back_icon_selected, 28-5, 400-332-25+4);
                drawImage(&about_image, 28-5, 400-48-180+4);
                drawImage(&about_mini, 31-5, 400-333-25+4);

                next_lcd_screen = LCD_ABOUT_SCREEN;
                displayUpdate();

              }
            else if (event == evt_Button_SELECT && cursor_pos == 1) {
//                next_lcd_screen = LCD_HOME_SCREEN;

                if(sensor_data->settings_unit == CM)
                  {
                    sensor_data->settings_unit = INCH;
                  }
                else
                  {
                    sensor_data->settings_unit = CM;
                  }

                clear_image(&inch, 95-5, 400-200-29); //inch being the widest, any unit with this would be cleared
                if(sensor_data->settings_unit == CM)
                {
                    measurement_unit = &cm;
                  drawImage(&cm, 101-5, 400-196-29);
                }
                else if(sensor_data->settings_unit == INCH)
                {
                    measurement_unit = &inch;
                  drawImage(&inch, 95-5, 400-196-29);
                }

//                update_home_screen(&stateMachine, rotate_counter);
                displayUpdate();
            }
            else if (event == evt_Button_SELECT && cursor_pos == 2)
              {
                clear_image(&back_icon_selected, 28-5, 400-332-25+4);
                clear_image(&back_icon, 28-5, 400-332-27+4);
                clear_image(&unit, 28-5, 400-93-38+4);
                next_lcd_screen = LCD_HOME_SCREEN;
                handle_peripherals_for_readings_menu(LCD_HOME_SCREEN);
                clear_image(&stateMachine.menu.items[stateMachine.menu.currentIndex].image, 28 + 38-5, 400-332-28);
                back_enabled = false;
                cursor_pos = 0;

//                Clearing unit
                clear_image(&inch, 95-5, 400-200-29); //inch being the widest, any unit with this would be cleared
                update_home_screen(&stateMachine, rotate_counter);
                displayUpdate();
              }

            if (event == evt_Button_UP || event == evt_Button_DOWN)
              {
            if(event == evt_Button_UP)
              {
//                increment cursor
                cursor_pos = (cursor_pos + 1) % 3;
              }
            else if(event == evt_Button_DOWN)
              {
//                decrement cursor
                cursor_pos = ((cursor_pos - 1)+3) % 3;
              }
               if(cursor_pos == 0)
                 {
                   clear_image(&dot, 194-5, 400-122-15 + 8);
                   drawImage(&dot, 194-5, 400-78-15 + 8);
                   back_enabled = false;
                   clear_image(&inch, 95-5, 400-200-29); //inch being the widest, any unit with this would be cleared
                 }
               else if(cursor_pos == 1)
                 {
                   clear_image(&dot, 194-5, 400-78-15 + 8);
                   drawImage(&dot, 194-5, 400-122-15 + 8);
                   back_enabled = false;
                   if(sensor_data->settings_unit == CM)
                    {
                      measurement_unit = &cm;
                      drawImage(measurement_unit, 101-5, 400-196-29);
                    }
                    else if(sensor_data->settings_unit == INCH)
                      {
                        measurement_unit = &inch;
                        drawImage(measurement_unit, 95-5, 400-196-29);
                      }
                 }
               else if(cursor_pos == 2)
                 {
                   clear_image(&dot, 194-5, 400-78-15 + 8);
                   clear_image(&dot, 194-5, 400-122-15 + 8);
                   back_enabled = true;
                   clear_image(&inch, 95-5, 400-200-29); //inch being the widest, any unit with this would be cleared
                 }

             if(back_enabled == true)
                 {
                   clear_image(&back_icon, 28-5, 400-332-29+4);
                   drawImage(&back_icon_selected, 28-5, 400-334-25+4);
                 }
               else
                 {
                   clear_image(&back_icon_selected, 28-5, 400-332-27+4);
                   drawImage(&back_icon, 28-5, 400-332-27+4);
                 }

//                  update_home_screen(&stateMachine, rotate_counter);
               displayUpdate();
              }

//            if (event == evt_Button_UP || event == evt_Button_DOWN)
//              {
//                clear_image(&dot, 196, 400-93-38 + 8);
//                  if(back_enabled == false)
//                    {
//                      clear_image(&back_icon, 28, 400-332-29);
//                      drawImage(&back_icon_selected, 28, 400-334-25);
//                      back_enabled = true;
//                    }
//                  else
//                    {
//                      clear_image(&back_icon_selected, 28, 400-332-27);
//                      drawImage(&back_icon, 28, 400-332-27);
//                      drawImage(&dot, 196, 400-93-38 + 8);
//                      back_enabled = false;
//                    }
//
////                  update_home_screen(&stateMachine, rotate_counter);
//                  displayUpdate();
//              }
       break;

        case LCD_ABOUT_SCREEN:
                    stateMachine.currentScreenType = LCD_ABOUT_SCREEN;
//                    if (event == evt_Button_SELECT && back_enabled == true) {
//                        clear_image(&back_icon_selected, 28, 400-332-25);
//                        clear_image(&back_icon, 28, 400-332-27);
//                        next_lcd_screen = LCD_HOME_SCREEN;
//                        handle_peripherals_for_readings_menu(LCD_HOME_SCREEN);
//                        clear_image(&stateMachine.menu.items[stateMachine.menu.currentIndex].image, 28 + 38, 400-332-28);
//                        back_enabled = false;
//
//        //                Clearing unit
//                        clear_image(&inch, 95, 400-200-29); //inch being the widest, any unit with this would be cleared
//                        update_home_screen(&stateMachine, rotate_counter);
//                        displayUpdate();
//                    }
                    if (event == evt_Button_UP || event == evt_Button_DOWN || event == evt_Button_SELECT)
                      {
//                        clear_menu_top_display();
                        clear_image(&about_image, 28-5, 400-48-180);
                        clear_image(&about_mini, 31-5, 400-333-25+4);
                        drawImage(&back_icon, 28-5, 400-334-25+4);
                        //drawImage(&change_unit, 28, 400-93-38);
                        drawImage(&about, 38-5, 400-34-56+4);
                        drawImage(&unit, 38-5, 400-96-33+4);
    //                    drawImage(&dot, 194, 400-107-15 + 8);
                        drawImage(&dot, 194-5, 400-78-15 + 8);

                        drawImage(&bluetooth_icon, 156-5, 400-333-27+4);
                        drawImage(&battery_icon, 185-5, 400-334-25+4);
                          next_lcd_screen = LCD_SETTINGS_SCREEN;
                          back_enabled = true;
                          if(back_enabled == false)
                            {
                              clear_image(&back_icon, 28-5, 400-332-29+4);
                              drawImage(&back_icon_selected, 28-5, 400-334-25+4);
                              back_enabled = true;
                            }
                          else
                            {
                              clear_image(&back_icon_selected, 28-5, 400-332-27+4);
                              drawImage(&back_icon, 28-5, 400-332-27+4);
                              back_enabled = false;
                            }


                          displayUpdate();
                      }
                    break;
    }
}











#if 0

void handle_lcd_menus(uint32_t event)
{

  //check if event is from switches
  //or from LCD timeout
  static uint32_t next_lcd_screen = 0;
  static uint32_t previous_lcd_screen = LCD_FLASH_SCREEN;
  sensor_data_t* sensor_data =  get_sensor_data();

  switch(previous_lcd_screen)
  {
    case LCD_FLASH_SCREEN:

      if((event == evt_Button_UP) ||
          (event == evt_Button_BACK)||
          (event == evt_Button_SELECT)||
          (event == evt_Button_DOWN))
      {
        next_lcd_screen = LCD_MENU_LINEAR;
        draw_custom_graphics(menu, RECTANGLE1);
      }

    break;
    
    case LCD_MENU_LINEAR:
      if(event == evt_Button_UP)
      {
        draw_custom_graphics(menu, RECTANGLE3);
        next_lcd_screen = LCD_MENU_SETTINGS;
      }
      else if(event == evt_Button_BACK)
      {
        //No action
      }
      else if(event == evt_Button_SELECT)
      {
        draw_custom_graphics(linear, RECTANGLE1);

        next_lcd_screen = LCD_LINEAR_STRING;
      }
      else if(event == evt_Button_DOWN)
      {
        draw_custom_graphics(menu, RECTANGLE2);
        next_lcd_screen = LCD_MENU_ANGULAR;
      }

    break;

    case LCD_LINEAR_STRING:
      if(event == evt_Button_UP)
      {
        draw_custom_graphics(linear, RECTANGLE3);
        next_lcd_screen = LCD_LINEAR_SONIC;
      }
      else if(event == evt_Button_BACK)
      {
        draw_custom_graphics(menu, RECTANGLE1);
        next_lcd_screen = LCD_MENU_LINEAR;
      }
      else if(event == evt_Button_SELECT)
      {
          /*code for enabling magnetic encoder and enable auto_updating_display*/
        //Turn magnetic encoder ON
          gpioMagEncPSetOn();
        sensor_data->is_mag_enc_string = true;
        sensor_data->is_mag_enc_enabled = true;
        sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
        //set flag that would keep updating measured values using soft_timer
        //if setting is to use cm as unit
        draw_custom_graphics(string, NO_RECTANGLE);
        //Add CM or INCH
        next_lcd_screen = LCD_STRING_MEAS;
        //if setting is to use cm as unit
        // draw_custom_graphics(string_meas_inch);
        if(sensor_data->settings_unit==CM)
            displayUnit(DISPLAY_ROW_BTADDR, "cm");
        else if(sensor_data->settings_unit==INCH)
            displayUnit(DISPLAY_ROW_BTADDR, "inch");
      }
      else if(event == evt_Button_DOWN)
      {
        draw_custom_graphics(linear, RECTANGLE2);
//        EMSTATUS GLIB_drawRect(GLIB_Context_t *pContext, const GLIB_Rectangle_t *pRect);
        next_lcd_screen = LCD_LINEAR_WHEEL;
      }
    break;

    case LCD_STRING_MEAS:
      if(event == evt_Button_UP)
      {
        //No action
      }
      else if(event == evt_Button_BACK)
      {
          gpioMagEncPSetOff();
        sensor_data->is_mag_enc_enabled = false;  // move this to gpioMagEncSetOff()
        draw_custom_graphics(linear, RECTANGLE1);
        sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
        sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM2);
        next_lcd_screen = LCD_LINEAR_STRING;
      }
      else if(event == evt_Button_SELECT)
      {
//          gpioMagEncPSetOff();
          //saving parameter in the database. To be sent over bluetooth
          sensor_data->mag_enc_reading_to_be_saved = sensor_data->refreshing_mag_enc_reading;
          ble_SendMeasurement(LCD_STRING_MEAS, (float)((float)sensor_data->mag_enc_reading_to_be_saved / 100.0));
          //ble_SendTemp((float)((float)sensor_data->mag_enc_reading_to_be_saved / 100.0));
//          sensor_data->is_mag_enc_enabled = false;
        //No action
      }
      else if(event == evt_Button_DOWN)
      {
        //No action
      }
    break;
    

    case LCD_LINEAR_WHEEL:
      if(event == evt_Button_UP)
      {
        draw_custom_graphics(linear, RECTANGLE1);
        next_lcd_screen = LCD_LINEAR_STRING;
      }
      else if(event == evt_Button_BACK)
      {
        draw_custom_graphics(menu, RECTANGLE1);
        next_lcd_screen = LCD_MENU_LINEAR;
      }
      else if(event == evt_Button_SELECT)
      {
          /*code for enabling magnetic encoder and enable auto_updating_display*/
        //Turn magnetic encoder ON
          gpioMagEncWSetOn();
        sensor_data->is_mag_enc_string = false;
        sensor_data->is_mag_enc_enabled = true;
        sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
        //set flag that would keep updating measured values using soft_timer

        //if unit is set as cm
        draw_custom_graphics(wheel, NO_RECTANGLE);
        //Add CM or INCH text
        next_lcd_screen = LCD_WHEEL_MEAS;
        curr_lcd_screen = LCD_WHEEL_MEAS;
        //if unit is set as inch
        //draw_custom_graphics(wheel_meas_inch);
        if(sensor_data->settings_unit==CM)
            displayUnit(DISPLAY_ROW_BTADDR, "cm");
        else if(sensor_data->settings_unit==INCH)
            displayUnit(DISPLAY_ROW_BTADDR, "inch");
      }
      else if(event == evt_Button_DOWN)
      {
        draw_custom_graphics(linear, RECTANGLE3);
        next_lcd_screen = LCD_LINEAR_SONIC;
      }
    break;

    case LCD_WHEEL_MEAS:
      if(event == evt_Button_UP)
      {
        //No action
      }
      else if(event == evt_Button_BACK)
      {
          gpioMagEncWSetOff();
        sensor_data->is_mag_enc_enabled = false;
        draw_custom_graphics(linear, RECTANGLE2);
        sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
        sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM2);
        next_lcd_screen = LCD_LINEAR_WHEEL;
        curr_lcd_screen = 0;
      }
      else if(event == evt_Button_SELECT)
      {
          gpioMagEncWSetOff();
          //saving parameter in the database. To be sent over bluetooth
          sensor_data->mag_enc_reading_to_be_saved = sensor_data->refreshing_mag_enc_reading;
          sensor_data->is_mag_enc_enabled = false;
          ble_SendMeasurement(LCD_WHEEL_MEAS, (float)((float)sensor_data->mag_enc_reading_to_be_saved / 100.0));
          curr_lcd_screen = 0;
        //No action
      }
      else if(event == evt_Button_DOWN)
      {
        //No action
      }
    break;


    case LCD_LINEAR_SONIC: 
      if(event == evt_Button_UP)
      {
        next_lcd_screen = LCD_LINEAR_WHEEL;
        draw_custom_graphics(linear, RECTANGLE2);
      }
      else if(event == evt_Button_BACK)
      {
        next_lcd_screen = LCD_MENU_LINEAR;
        draw_custom_graphics(menu, RECTANGLE1);
      }
      else if(event == evt_Button_SELECT)
      {
          gpioUltrasonicSetOn();
          sensor_data->is_ultrasonic_enabled = true;
        //if unit is set as cm
        next_lcd_screen = LCD_SONIC_MEAS;
        //Add CM or INCH
        draw_custom_graphics(sonic, NO_RECTANGLE);
        if(sensor_data->settings_unit==CM)
            displayUnit(DISPLAY_ROW_BTADDR, "cm");
        else if(sensor_data->settings_unit==INCH)
            displayUnit(DISPLAY_ROW_BTADDR, "inch");

        //if unit is set as inch
        // draw_custom_graphics(sonic_meas_inch);
      }
      else if(event == evt_Button_DOWN)
      {
        next_lcd_screen = LCD_LINEAR_STRING;
        draw_custom_graphics(linear, RECTANGLE1);
      }
    break;

    case LCD_SONIC_MEAS:
      if(event == evt_Button_UP)
      {
        //No action
      }
      else if(event == evt_Button_BACK)
      {
        gpioUltrasonicSetOff();
        sensor_data->is_ultrasonic_enabled = false;
        clear_display();
        //add delay to make sure display has been updated
        next_lcd_screen = LCD_LINEAR_SONIC;
        draw_custom_graphics(linear, RECTANGLE3);
      }
      else if(event == evt_Button_SELECT)
      {
          sensor_data->ultrasonic_reading_to_be_saved = sensor_data->refreshing_ultrasonic_reading;
          ble_SendMeasurement(LCD_SONIC_MEAS, (float)((float)sensor_data->ultrasonic_reading_to_be_saved));
        //No action
      }
      else if(event == evt_Button_DOWN)
      {
        //No action
      }
    break;
    

    case LCD_MENU_ANGULAR: 
      if(event == evt_Button_UP)
      {
        next_lcd_screen = LCD_MENU_LINEAR;
        draw_custom_graphics(menu, RECTANGLE1);
      }
      else if(event == evt_Button_BACK)
      {
        //No action
      }
      else if(event == evt_Button_SELECT)
      {
        sensor_data->is_angular_enabled = true;



        //if unit is set as cm

        next_lcd_screen = LCD_ANGULAR_MEAS;
        draw_custom_graphics(angular, NO_RECTANGLE);
        imu_init_state_machine(evt_NoEvent);
      }
      else if(event == evt_Button_DOWN)
      {
        next_lcd_screen = LCD_MENU_SETTINGS;
        draw_custom_graphics(menu, RECTANGLE3);
      }
    break;

    case LCD_ANGULAR_MEAS: 
      if(event == evt_Button_UP)
      {
        //No action
      }
      else if(event == evt_Button_BACK)
      {
        next_lcd_screen = LCD_MENU_ANGULAR;
        sensor_data->is_angular_enabled = false;
        sensor_data->is_imu_initialization_complete = false;
        sensor_data->is_imu_reading_ready = false;
//        timerWaitUs_polled(50000);
//        sensor_data->should_refresh_imu_data = false;
        //enter IMU low power
//        imuSetOff();
        imu_turnoff_state_machine(evt_NoEvent);
        draw_custom_graphics(menu, RECTANGLE2);
      }
      else if(event == evt_Button_SELECT)
      {
          sensor_data->angular_reading_to_be_saved = sensor_data->refreshing_angular_reading;
          ble_SendMeasurement(LCD_ANGULAR_MEAS, (float)((float)abs(sensor_data->angular_reading_to_be_saved)));
        //No action
      }
      else if(event == evt_Button_DOWN)
      {
        //No action
      }
    break;

    case LCD_MENU_SETTINGS: 
      if(event == evt_Button_UP)
      {
        next_lcd_screen = LCD_MENU_ANGULAR;
        draw_custom_graphics(menu, RECTANGLE2);

      }
      else if(event == evt_Button_BACK)
      {
        //No action
      }
      else if(event == evt_Button_SELECT)
      {
        next_lcd_screen = LCD_SETTINGS_UNIT;
        draw_custom_graphics(settings, RECTANGLE1);
      }
      else if(event == evt_Button_DOWN)
      {
        next_lcd_screen = LCD_MENU_LINEAR;
        draw_custom_graphics(menu, RECTANGLE1);
      }
    break;

    case LCD_SETTINGS_UNIT: 
      if(event == evt_Button_UP)
      {
        next_lcd_screen = LCD_SETTINGS_CHARGE_STATUS;
        draw_custom_graphics(settings, RECTANGLE2);
      }
      else if(event == evt_Button_BACK)
      {
        next_lcd_screen = LCD_MENU_SETTINGS;
        draw_custom_graphics(menu, RECTANGLE3);
      }
      else if(event == evt_Button_SELECT)
      {
        next_lcd_screen = LCD_UNIT_CM;
        draw_custom_graphics(unit, RECTANGLE1);
      }
      else if(event == evt_Button_DOWN)
      {
        next_lcd_screen = LCD_SETTINGS_CHARGE_STATUS;
        draw_custom_graphics(settings, RECTANGLE2);
      }
    break;

    case LCD_UNIT_CM: 
      if(event == evt_Button_UP)
      {
        next_lcd_screen = LCD_UNIT_INCH;
        draw_custom_graphics(unit, RECTANGLE2);
      }
      else if(event == evt_Button_BACK)
      {
        next_lcd_screen = LCD_SETTINGS_UNIT;
        draw_custom_graphics(settings, RECTANGLE1);
      }
      else if(event == evt_Button_SELECT)
      {
        sensor_data->settings_unit = CM; //change this with enum
        ble_SendMeasurement(LCD_SETTINGS_UNIT, (float)sensor_data->settings_unit);
        next_lcd_screen = LCD_SETTINGS_UNIT;
        //set the unit to cm
        draw_custom_graphics(settings, RECTANGLE1);
      }
      else if(event == evt_Button_DOWN)
      {
        next_lcd_screen = LCD_UNIT_INCH;
        draw_custom_graphics(unit, RECTANGLE2);
      }
    break;

    case LCD_UNIT_INCH: 
      if(event == evt_Button_UP)
      {
        next_lcd_screen = LCD_UNIT_CM;
        draw_custom_graphics(unit, RECTANGLE1);
      }
      else if(event == evt_Button_BACK)
      {
        next_lcd_screen = LCD_SETTINGS_UNIT;
        draw_custom_graphics(settings, RECTANGLE1);
      }
      else if(event == evt_Button_SELECT)
      {
        sensor_data->settings_unit = INCH; //change this with enum
        ble_SendMeasurement(LCD_SETTINGS_UNIT, (float)sensor_data->settings_unit);
        next_lcd_screen = LCD_SETTINGS_UNIT;
        draw_custom_graphics(settings, RECTANGLE1);
      }
      else if(event == evt_Button_DOWN)
      {
        next_lcd_screen = LCD_UNIT_CM;
        draw_custom_graphics(unit, RECTANGLE1);
      }
    break;

    case LCD_SETTINGS_CHARGE_STATUS: 
      if(event == evt_Button_UP)
      {
        next_lcd_screen = LCD_SETTINGS_UNIT;
        draw_custom_graphics(settings, RECTANGLE1);
      }
      else if(event == evt_Button_BACK)
      {
        next_lcd_screen = LCD_MENU_SETTINGS;
        draw_custom_graphics(menu, RECTANGLE3);
      }
      else if(event == evt_Button_SELECT)
      {
        next_lcd_screen = LCD_CHARGE_STATUS_ON;
        draw_custom_graphics(charge_status, RECTANGLE2);
      }
      else if(event == evt_Button_DOWN)
      {
        next_lcd_screen = LCD_SETTINGS_UNIT;
        draw_custom_graphics(settings, RECTANGLE1);
      }
    break;

    case LCD_CHARGE_STATUS_ON: 
      if(event == evt_Button_UP)
      {
        next_lcd_screen = LCD_CHARGE_STATUS_OFF;
        draw_custom_graphics(charge_status, RECTANGLE3);
      }
      else if(event == evt_Button_BACK)
      {
        next_lcd_screen = LCD_SETTINGS_CHARGE_STATUS;
        draw_custom_graphics(settings, RECTANGLE2);
      }
      else if(event == evt_Button_SELECT)
      {
        next_lcd_screen = LCD_SETTINGS_CHARGE_STATUS;
        //set LCD On while charging
        draw_custom_graphics(settings, RECTANGLE2);
      }
      else if(event == evt_Button_DOWN)
      {
        next_lcd_screen = LCD_CHARGE_STATUS_OFF;
        draw_custom_graphics(charge_status, RECTANGLE3);
      }
    break;

    case LCD_CHARGE_STATUS_OFF: 
      if(event == evt_Button_UP)
      {
        next_lcd_screen = LCD_CHARGE_STATUS_ON;
        draw_custom_graphics(charge_status, RECTANGLE2);
      }
      else if(event == evt_Button_BACK)
      {
        next_lcd_screen = LCD_SETTINGS_CHARGE_STATUS;
        draw_custom_graphics(settings, RECTANGLE2);
      }
      else if(event == evt_Button_SELECT)
      {
        next_lcd_screen = LCD_SETTINGS_CHARGE_STATUS;
        //set LCD Off while charging
        draw_custom_graphics(settings, RECTANGLE2);
      }
      else if(event == evt_Button_DOWN)
      {
        next_lcd_screen = LCD_CHARGE_STATUS_ON;
        draw_custom_graphics(charge_status, RECTANGLE2);
      }
    break;
  }

  previous_lcd_screen = next_lcd_screen;
  
  displayUpdate();


}
#endif


/********************************************************************************
 *
 *******************************************************************************/

void lcd_auto_update_display()
{
  sensor_data_t* sensor_data =  get_sensor_data();
  char refreshing_reading_str[5] = "";


  if(sensor_data->is_mag_enc_enabled == true)
    {
      if(sensor_data->is_mag_enc_string == false)
        {
          if(sensor_data->settings_unit == CM)
            sensor_data->refreshing_mag_enc_reading = (abs(mag_enc.clkwise_counter - mag_enc.counter_clkwise_counter)*28.43/10); //1 pulse corresponds to 0.2843 (200 pulses correspond to pi*18.1mm... multiplied by 100 to improve resolution
          else if((sensor_data->settings_unit == INCH))
            sensor_data->refreshing_mag_enc_reading = (abs(mag_enc.clkwise_counter - mag_enc.counter_clkwise_counter)*28.43*0.3937/10); //1 pulse corresponds to 0.2843 (200 pulses correspond to pi*18.1mm... multiplied by 100 to improve resolution
        }
      else
        {
          if(sensor_data->settings_unit == CM)
            sensor_data->refreshing_mag_enc_reading = (abs(mag_enc.clkwise_counter - mag_enc.counter_clkwise_counter)*70.68/10); //1 pulse corresponds to 17.67mm (200 pulses correspond to pi*45mm... multiplied by 100 to improve resolution
          else if((sensor_data->settings_unit == INCH))
            sensor_data->refreshing_mag_enc_reading = (abs(mag_enc.clkwise_counter - mag_enc.counter_clkwise_counter)*70.68*0.3937/10); //1 pulse corresponds to 17.67mm (200 pulses correspond to pi*45mm... multiplied by 100 to improve resolution
        }

      if(sensor_data->refreshing_mag_enc_reading < 100)
        sensor_data->refreshing_mag_enc_reading = 0;

//      sprintf(refreshing_reading_str,"%3d.%2d",(int)(sensor_data->refreshing_mag_enc_reading/100), ((int)(sensor_data->refreshing_mag_enc_reading) % 100));
      display_reading((int)(sensor_data->refreshing_mag_enc_reading), false);
//      displayPrintf(DISPLAY_ROW_CLIENTADDR, (refreshing_reading_str));
//      displayPrintf(DISPLAY_ROW_CLIENTADDR, "%f", b);//sensor_data->refreshing_mag_enc_reading/100);
      //write mag_enc numbers on the LCD
    }
  else if(sensor_data->is_ultrasonic_enabled == true)
    {
      //ultrasonic_readings
      for(int i=0; i< 15; i++)  // processing entire circular buffer
       {
          sensor_data->refreshing_ultrasonic_reading = get_processed_data_from_ultrasonic_sensor();
          if(sensor_data->refreshing_ultrasonic_reading != 0)
            {
              if((sensor_data->settings_unit == CM))  //checking only if settings is in CM. By default reading is received is in INCH
                sensor_data->refreshing_ultrasonic_reading = (int)(sensor_data->refreshing_ultrasonic_reading*2.54);
              break;
            }
       }
      if((sensor_data->refreshing_ultrasonic_reading != 0) && (sensor_data->refreshing_ultrasonic_reading != 255))
      {

              if((sensor_data->refreshing_ultrasonic_reading > 6) && (sensor_data->settings_unit == INCH))
                {
                  sprintf(refreshing_reading_str,"%3d",(int)(sensor_data->refreshing_ultrasonic_reading));
//                  displayPrintf(DISPLAY_ROW_CLIENTADDR, (refreshing_reading_str));
                }
              else if((sensor_data->refreshing_ultrasonic_reading > 15) && (sensor_data->settings_unit == CM))
                {
                  sprintf(refreshing_reading_str,"%3d",(int)(sensor_data->refreshing_ultrasonic_reading));
//                  displayPrintf(DISPLAY_ROW_CLIENTADDR, (refreshing_reading_str));
                }
              else
                {
//                  displayPrintf(DISPLAY_ROW_CLIENTADDR, "...");
                }

              display_reading((int)(sensor_data->refreshing_ultrasonic_reading), true);

      }
      //write ultrasonic numbers on the LCD
    }

  else if(sensor_data->is_angular_enabled == true)
    {
      if(sensor_data->is_imu_initialization_complete == true)
        {
          if(sensor_data->is_imu_reading_ready == true)
            {
//              sprintf(refreshing_reading_str,"%3d",(int)(sensor_data->refreshing_angular_reading));
              display_reading((int)(sensor_data->refreshing_angular_reading), true);
//              displayPrintf(DISPLAY_ROW_CLIENTADDR, (refreshing_reading_str));
              sensor_data->is_imu_reading_ready = false;
              schedulerSetEventStartNextImuReading();

            }
        }

//      sensor_data->should_refresh_imu_data = true;
//      imu_data_state_machine(state0_i2c_read);//write BNO numbers on the LCD

    }
}

