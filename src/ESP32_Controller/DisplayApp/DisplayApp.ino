/*
 Name:		DisplayApp.ino
 Created:	09.10.2025 20:45:03
 Author:	Mario
*/

#include "COMMON_DEFINES.h"

#include <Arduino.h>
#include <Wire.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include "ui.h"


//Touchscreen
SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(TFT_DISPLAY_TOUCH_CS_PIN);
int touch_x, touch_y, touch_z;

//LVGL
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];




void setup() 
{
    //Init Serial
    delay(1000);
    Serial.begin(115200);
    while (!Serial) {}
    delay(1000);

    //Set all chip selects high to avoid bus contention during initialisation of each peripheral
    digitalWrite(TFT_DISPLAY_TOUCH_CS_PIN, HIGH); // Touch controller chip select (if used)
    digitalWrite(TFT_DISPLAY_CS_PIN, HIGH); // TFT screen chip select
    digitalWrite(SD_CARD_CS_PIN, HIGH); // SD card chips select, must use GPIO 5 (ESP32 SS)
    delay(200);

    InitGui();
    delay(200);


    delay(200);
    Serial.println("Setup Done!");
}


//MAIN LOOP
void loop() 
{
    lv_task_handler();//LVGL
    ui_tick();//EEZ Studio UI
    lv_tick_inc(2);//LVGL
    delay(2);
}














//Init LVGL & EEZ Studio Stuff
void InitGui()
{
    //Init LVGL
    lv_init();

    // Start the SPI for the touchscreen and init the touchscreen
    touchscreenSPI.begin(TFT_DISPLAY_SCK, TFT_DISPLAY_MISO, TFT_DISPLAY_MOSI, TFT_DISPLAY_TOUCH_CS_PIN);
    touchscreen.begin(touchscreenSPI);
    touchscreen.setRotation(2);

    // Create a display object
    lv_display_t* disp;
    disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
    lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_0);

    // Init an LVGL input device object (in this case a Touchscreen)
    lv_indev_t* indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touchscreen_read);

    //Init EEZ Studio UI
    ui_init();
    delay(200);

    //Menu Button Events
    lv_obj_add_event_cb(objects.button_open_menu, main_button_event_cb, LV_EVENT_ALL, NULL);//Open Menu Button
    lv_obj_add_event_cb(objects.button_exit_menu, main_button_event_cb, LV_EVENT_ALL, NULL);//Close Menu Button
    lv_obj_add_event_cb(objects.menu_main_opt_open_dev_menu, main_button_event_cb, LV_EVENT_ALL, NULL);//Open DevMenu Button
    lv_obj_add_event_cb(objects.button_back_menu_dev, main_button_event_cb, LV_EVENT_ALL, NULL);//Go Back Button(DevMenu)
}


//Get Touchscreen Info for LVGL
void touchscreen_read(lv_indev_t* indev, lv_indev_data_t* data)
{
    if (touchscreen.tirqTouched() && touchscreen.touched())
    {
        // Get Touchscreen points
        TS_Point touchPoint = touchscreen.getPoint();
        // Calibrate Touchscreen points with map function to the correct width and height
        touch_x = map(touchPoint.x, 200, 3700, 1, SCREEN_WIDTH);
        touch_y = map(touchPoint.y, 240, 3800, 1, SCREEN_HEIGHT);
        touch_z = touchPoint.z;
        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = touch_x;
        data->point.y = touch_y;
    }
    else
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}


//Main Button Event
static void main_button_event_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target_obj(e);

    if (code == LV_EVENT_CLICKED)
    {
        if (btn == objects.button_open_menu)//Open Menu Button Clicked
        {
            changeToNextScreen(SCREEN_ID_MENU_MAIN);
        }
        else if (btn == objects.button_exit_menu)//Close Menu Button Clicked
        {
            changeToPrevScreen(SCREEN_ID_INFO_SCREEN);
        }
        else if (btn == objects.menu_main_opt_open_dev_menu)//Open Developer Menu Button Clicked
        {
            changeToNextScreen(SCREEN_ID_MENU_DEV);
        }
        else if (btn == objects.button_back_menu_dev)//Go Back Button Clicked --- DevMenu
        {
            changeToPrevScreen(SCREEN_ID_MENU_MAIN);
        }
    }
    else
    {
    }
}
