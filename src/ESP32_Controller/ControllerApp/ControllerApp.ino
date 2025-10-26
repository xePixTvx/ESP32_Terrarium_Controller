/*
 Name:		ControllerApp.ino
 Created:	06.08.2025 21:41:41
 Author:	Mario
*/

//LAST WORKED ON: GUI Light Settings Menu --- Config stuff needs to work first!!!!!
//                Config Controller ------ Read File Stuff




/*
*           TODO:
*                   LVGL                                    -------------------------------- DONE ------- WORKS YAY
*                   EEZ Studio Implementation               -------------------------------- DONE ------- WORKS YAY
*                   FanControl                              -------------------------------- RPM Readings are still not 100% working
*                   Controller Temp Sensor                  -------------------------------- DONE ------- WORKS YAY
*                   Terrarium SHT Temp & Humidity Sensor    -------------------------------- DONE ------- WORKS YAY
*                   Terrarium Ground Humidity Sensor        -------------------------------- NOT STARTED
*                   Terrarium Light & Heater                -------------------------------- DONE ------- WORKS YAY
*                   RTC(DS3231)                             -------------------------------- DONE ------- WORKS YAY
*                   Config/Settings using LittleFS          -------------------------------- WIP
*/
#include "COMMON_DEFINES.h"

#include <Arduino.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SHT31.h>



#if DISABLE_UI_AND_TOUCH != true
    #include <lvgl.h>
    #include <TFT_eSPI.h>
    #include <XPT2046_Touchscreen.h>
    #include "ui.h"
#endif



#include "ConfigControl.h"
#include "SensorUpdater.h"
#include "ClockControl.h"
#include "FanControl.h"
#include "LightHeaterControl.h"


//Config Controller
ConfigControl ConfigController = ConfigControl();

#if DISABLE_UI_AND_TOUCH != true
    //Touchscreen
    SPIClass touchscreenSPI = SPIClass(VSPI);
    XPT2046_Touchscreen touchscreen(TFT_DISPLAY_TOUCH_CS_PIN);
    int touch_x, touch_y, touch_z;

    //LVGL
    #define SCREEN_WIDTH 240
    #define SCREEN_HEIGHT 320
    #define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
    uint32_t draw_buf[DRAW_BUF_SIZE / 4];
#endif

//Update Task Delay Setting
static bool AllowUpdaterTask = false;

//One Wire Bus
OneWire OneWireBus(ONEWIRE_PIN);
DallasTemperature OneWireTempSensors(&OneWireBus);

//SHT31 Temp & Humidity Sensor
SHT31 ShtSensor;

//Sensor Updater
SensorUpdater SensorUpd = SensorUpdater(&OneWireTempSensors, &ShtSensor);

//Clock Controller
ClockControl ClockCtrl = ClockControl();

//Fan Controller
FanControl FanController = FanControl();

//Light & Heater Controller
LightHeaterControl LightHeaterController = LightHeaterControl();


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

    #if DISABLE_UI_AND_TOUCH != true
        InitGui();
        delay(200);
    #endif

    //Init Clock Controller
    ClockCtrl.Begin();
    delay(200);

    //Init Config Controller
    ConfigController.Begin();
    delay(200);

    //Create UpdaterTask
    xTaskCreate(UpdateTask, "UpdateTask", 4096, NULL, 2, NULL);
    delay(200);

    //Init Sensor Updater
    SensorUpd.Begin();
    delay(200);

    //Init Fan Controller
    FanController.Begin();
    FanController.SetSpeedPercent(0, 0);
    FanController.SetSpeedPercent(1, 0);

    //Init Light & Heater Controller
    LightHeaterController.Begin();

    Serial.println("Setup Done!");
    delay(200);
}






//MAIN LOOP
void loop()
{
    //RTC Update
    ClockCtrl.Update();


    //Start Sensor Updating after delay
    if (!AllowUpdaterTask && (millis() >= UPDATE_TASK_START_DELAY))
    {
        SensorUpd.Start();
        AllowUpdaterTask = true;
    }




    //LVGL/EEZ GUI Stuff
    #if DISABLE_UI_AND_TOUCH != true
        lv_task_handler();//LVGL
        ui_tick();//EEZ Studio UI
        lv_tick_inc(2);//LVGL
        delay(2);
    #endif
}


//Update Task LOOP
void UpdateTask(void* arg)
{
    while (1)
    {
        if (AllowUpdaterTask)
        {
            //Update Sensor Updater
            SensorUpd.Update();

            //Update Fan Controller
            FanController.Update();
        }
        delay(2);
    }
}






#if DISABLE_UI_AND_TOUCH != true
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

    //Menu Events
    SetupMenuEvents();
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




//Menu Options Stuff
void SetupMenuEvents()
{
    //Info Screen
    lv_obj_add_event_cb(objects.button_open_menu, main_button_event_cb, LV_EVENT_ALL, NULL);//Open Menu Button

    //Main Menu
    lv_obj_add_event_cb(objects.button_exit_menu, main_button_event_cb, LV_EVENT_ALL, NULL);//Close Menu Button
    lv_obj_add_event_cb(objects.menu_main_opt_open_dev_menu, main_button_event_cb, LV_EVENT_ALL, NULL);//Open DevMenu Button
    lv_obj_add_event_cb(objects.menu_main_button_set_time_ndate, main_button_event_cb, LV_EVENT_ALL, NULL);//Open Set Time & Date Menu
    lv_obj_add_event_cb(objects.menu_main_opt_light_settings, main_button_event_cb, LV_EVENT_ALL, NULL);//Open Light Settings Menu

    //Time & Date Menu
    lv_obj_add_event_cb(objects.menu_time_ndate_button_cancel, main_button_event_cb, LV_EVENT_ALL, NULL);//Cancel Button
    lv_obj_add_event_cb(objects.menu_time_ndate_button_time_next, main_button_event_cb, LV_EVENT_ALL, NULL);//Next Button
    lv_obj_add_event_cb(objects.menu_time_ndate_button_save, main_button_event_cb, LV_EVENT_ALL, NULL);//Save Button
    lv_obj_add_event_cb(objects.menu_time_ndate_button_prev, main_button_event_cb, LV_EVENT_ALL, NULL);//Prev Button

    //Light Settings Menu
    lv_obj_add_event_cb(objects.button_back_menu_light_settings, main_button_event_cb, LV_EVENT_ALL, NULL);//Go Back Button
    lv_obj_add_event_cb(objects.menu_light_settings_button_save, main_button_event_cb, LV_EVENT_ALL, NULL);//Save Button

    //Dev Menu
    lv_obj_add_event_cb(objects.button_back_menu_dev, main_button_event_cb, LV_EVENT_ALL, NULL);//Go Back Button(DevMenu)
    lv_obj_add_event_cb(objects.menu_dev_opt_reset_clock, main_button_event_cb, LV_EVENT_ALL, NULL);//Reset Clock Button
    lv_obj_add_event_cb(objects.menu_dev_opt_light_on, main_button_event_cb, LV_EVENT_ALL, NULL);//Turn Light on
    lv_obj_add_event_cb(objects.menu_dev_opt_light_off, main_button_event_cb, LV_EVENT_ALL, NULL);//Turn Light off
    lv_obj_add_event_cb(objects.menu_dev_opt_heater_on, main_button_event_cb, LV_EVENT_ALL, NULL);//Turn Heater on
    lv_obj_add_event_cb(objects.menu_dev_opt_heater_off, main_button_event_cb, LV_EVENT_ALL, NULL);//Turn Heater off
}







//Main Button Event
static void main_button_event_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* btn = lv_event_get_target_obj(e);

    if (code == LV_EVENT_CLICKED)
    {
        //Info Screen
        if (getCurrentScreen() == SCREEN_ID_INFO_SCREEN)
        {
            if (btn == objects.button_open_menu)//Open Menu Button Clicked
            {
                changeToNextScreen(SCREEN_ID_MENU_MAIN);
            }
        }


        //Main Menu
        if (getCurrentScreen() == SCREEN_ID_MENU_MAIN)
        {
            if (btn == objects.button_exit_menu)//Close Menu Button Clicked
            {
                changeToPrevScreen(SCREEN_ID_INFO_SCREEN);
            }
            else if (btn == objects.menu_main_opt_open_dev_menu)//Open Developer Menu Button Clicked
            {
                changeToNextScreen(SCREEN_ID_MENU_DEV);
            }
            else if (btn == objects.menu_main_button_set_time_ndate)//Open Set Time & Date Menu
            {
                //Set Rollers to Current Time And Date here
                DateTime dateTime = ClockCtrl.GetDateTime();
                lv_roller_set_selected(objects.roller_time_ndate_time_hours, (dateTime.hour() - 1), true);
                lv_roller_set_selected(objects.roller_time_ndate_time_minutes, (dateTime.minute() - 1), true);
                lv_roller_set_selected(objects.roller_time_ndate_date_day, (dateTime.day() - 1), true);
                lv_roller_set_selected(objects.roller_time_ndate_date_month, (dateTime.month() - 1), true);
                lv_roller_set_selected_str(objects.roller_time_ndate_date_year, String(dateTime.year()).c_str(), true);//does this really work??

                changeToNextScreen(SCREEN_ID_MENU_SETTINGS_TIME_NDATE);
            }
            else if (btn == objects.menu_main_opt_light_settings)//Open Light Settings Menu
            {
                changeToNextScreen(SCREEN_ID_MENU_SETTINGS_LIGHT);
            }
        }


        //Set Time & Date Menu
        if (getCurrentScreen() == SCREEN_ID_MENU_SETTINGS_TIME_NDATE)
        {
            if (btn == objects.menu_time_ndate_button_cancel)//Cancel Button
            {
                changeToPrevScreen(SCREEN_ID_MENU_MAIN);
            }
            else if (btn == objects.menu_time_ndate_button_time_next)//Next Button
            {
                changeToNextScreen(SCREEN_ID_MENU_SETTINGS_TIME_NDATE2);
            }
        }

        //Set Time & Date Menu 2
        if (getCurrentScreen() == SCREEN_ID_MENU_SETTINGS_TIME_NDATE2)
        {
            if (btn == objects.menu_time_ndate_button_save)//Save Button
            {
                //Get from GUI Rollers
                uint32_t hour = lv_roller_get_selected(objects.roller_time_ndate_time_hours) + 1;
                uint32_t minute = lv_roller_get_selected(objects.roller_time_ndate_time_minutes) + 1;
                uint32_t day = lv_roller_get_selected(objects.roller_time_ndate_date_day) + 1;
                uint32_t month = lv_roller_get_selected(objects.roller_time_ndate_date_month) + 1;
                
                //Get Year String and convert it to uint32_t
                char buf[5];
                lv_roller_get_selected_str(objects.roller_time_ndate_date_year, buf, 5);
                uint32_t year = atoi(buf);
                //Serial.println("YEAR: " + String(year));

                //Set Clock Time & Date and Restart ESP32
                ClockCtrl.SetTimeAndDate(year, month, day, hour, minute, 0);//Set Clock Time & Date
                ESP.restart();//RESTART ESP32
            }
            else if (btn == objects.menu_time_ndate_button_prev)//Previous Button
            {
                changeToPrevScreen(SCREEN_ID_MENU_SETTINGS_TIME_NDATE);
            }
        }


        //Light Settings Menu
        if (getCurrentScreen() == SCREEN_ID_MENU_SETTINGS_LIGHT)
        {
            if (btn == objects.button_back_menu_light_settings)//Go Back to Main Menu Button
            {
                changeToPrevScreen(SCREEN_ID_MENU_MAIN);
            }
            else if (btn == objects.menu_light_settings_button_save)//Save Button
            {
                uint32_t on_hour = lv_roller_get_selected(objects.roller_lights_on_time_hour) + 1;
                uint32_t on_minute = lv_roller_get_selected(objects.roller_lights_on_time_minute) + 1;
                uint32_t off_hour = lv_roller_get_selected(objects.roller_lights_off_time_hour) + 1;
                uint32_t off_minute = lv_roller_get_selected(objects.roller_lights_off_time_minute) + 1;

                String on_time_to_save = String(on_hour) + ":" + String(on_minute);
                String off_time_to_save = String(off_hour) + ":" + String(off_minute);
                //Serial.println(on_time_to_save);
                //delay(200);
            }
        }









        //Developer Menu
        if (getCurrentScreen() == SCREEN_ID_MENU_DEV)
        {
            if (btn == objects.button_back_menu_dev)//Go Back Button Clicked --- DevMenu
            {
                changeToPrevScreen(SCREEN_ID_MENU_MAIN);
            }
            else if (btn == objects.menu_dev_opt_reset_clock)//Reset Clock --- DevMenu
            {
                ClockCtrl.SetTimeAndDate(2000, 1, 1, 20, 15, 0);//Set Clock Time & Date
                ESP.restart();//RESTART ESP32
            }
            else if (btn == objects.menu_dev_opt_light_on)
            {
                LightHeaterController.SetLight(true);
            }
            else if (btn == objects.menu_dev_opt_light_off)
            {
                LightHeaterController.SetLight(false);
            }
            else if (btn == objects.menu_dev_opt_heater_on)
            {
                LightHeaterController.SetHeater(true);
            }
            else if (btn == objects.menu_dev_opt_heater_off)
            {
                LightHeaterController.SetHeater(false);
            }
        }
    }
    else
    {
    }
}




#endif