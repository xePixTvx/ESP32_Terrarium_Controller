/*
 Name:		ControllerApp.ino
 Created:	06.08.2025 21:41:41
 Author:	Mario
*/

//LAST WORKED ON: Time & Date GUI/Menu Stuff ---- Should be 2 menus ------ Currently not working



/*
*           TODO:
*                   SD Card Stuff                           -------------------------------- NEEDS TO BE RECODED
*                   LVGL                                    -------------------------------- DONE ------- WORKS YAY
*                   EEZ Studio Implementation               -------------------------------- DONE ------- WORKS YAY
*                   FanControl                              -------------------------------- RPM Readings are still not 100% working
*                   Controller Temp Sensor                  -------------------------------- DONE ------- Needs Testing
*                   Terrarium SHT Temp & Humidity Sensor    -------------------------------- DONE ------- Needs Testing
*                   Terrarium Ground Humidity Sensor        -------------------------------- NOT STARTED
*                   Terrarium Light & Heater                -------------------------------- NOT STARTED
*                   RTC(DS3231)                             -------------------------------- DONE ------- Should be working
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



#include "SensorUpdater.h"
#include "ClockControl.h"


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

    //Create UpdaterTask
    xTaskCreate(UpdateTask, "UpdateTask", 4096, NULL, 2, NULL);
    delay(200);

    //Init Sensor Updater
    SensorUpd.Begin();
    delay(200);

    Serial.println("Setup Done!");
    delay(200);
}






//MAIN LOOP
void loop()
{
    ClockCtrl.Update();


    if (!AllowUpdaterTask && (millis() >= UPDATE_TASK_START_DELAY))
    {
        SensorUpd.Start();
        AllowUpdaterTask = true;
    }





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
            //SensorUpd.Update();
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

    //Menu Options Stuff
    SetupMenuOptions();
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
void SetupMenuOptions()
{
    //Info Screen
    lv_obj_add_event_cb(objects.button_open_menu, main_button_event_cb, LV_EVENT_ALL, NULL);//Open Menu Button

    //Main Menu
    lv_obj_add_event_cb(objects.button_exit_menu, main_button_event_cb, LV_EVENT_ALL, NULL);//Close Menu Button
    lv_obj_add_event_cb(objects.menu_main_opt_open_dev_menu, main_button_event_cb, LV_EVENT_ALL, NULL);//Open DevMenu Button
    lv_obj_add_event_cb(objects.menu_main_button_set_time_ndate, main_button_event_cb, LV_EVENT_ALL, NULL);//Open Set Time & Date Menu

    //Time & Date Menu
    lv_obj_add_event_cb(objects.menu_time_ndate_button_cancel, main_button_event_cb, LV_EVENT_ALL, NULL);//Cancel Button
    lv_obj_add_event_cb(objects.menu_time_ndate_button_time_next, main_button_event_cb, LV_EVENT_ALL, NULL);//Next Button
    lv_obj_add_event_cb(objects.menu_time_ndate_button_save, main_button_event_cb, LV_EVENT_ALL, NULL);//Save Button
    lv_obj_add_event_cb(objects.menu_time_ndate_button_prev, main_button_event_cb, LV_EVENT_ALL, NULL);//Prev Button

    //Dev Menu
    lv_obj_add_event_cb(objects.button_back_menu_dev, main_button_event_cb, LV_EVENT_ALL, NULL);//Go Back Button(DevMenu)
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
                lv_roller_set_selected_str(objects.roller_time_ndate_date_year, String(dateTime.year()).c_str(), true);

                changeToNextScreen(SCREEN_ID_MENU_SETTINGS_TIME_NDATE);
            }
        }


        //Set Time & Date Menu
        if (getCurrentScreen() == SCREEN_ID_MENU_SETTINGS_TIME_NDATE)
        {
            if (btn == objects.menu_time_ndate_button_cancel)//Cancel Button
            {
                changeToPrevScreen(SCREEN_ID_MENU_MAIN);
            }
            else if (btn == objects.menu_time_ndate_button_time_next)
            {
                changeToNextScreen(SCREEN_ID_MENU_SETTINGS_TIME_NDATE2);
            }
        }

        //Set Time & Date Menu 2
        if (getCurrentScreen() == SCREEN_ID_MENU_SETTINGS_TIME_NDATE2)
        {
            if (btn == objects.menu_time_ndate_button_save)//Save Button
            {
                uint32_t hour = lv_roller_get_selected(objects.roller_time_ndate_time_hours);
                uint32_t minute = lv_roller_get_selected(objects.roller_time_ndate_time_minutes);
                uint32_t day = lv_roller_get_selected(objects.roller_time_ndate_date_day);
                uint32_t month = lv_roller_get_selected(objects.roller_time_ndate_date_month);
                
                char buf[4];
                lv_roller_get_selected_str(objects.roller_time_ndate_date_year, buf, 4);
                uint32_t year = atoi(buf);
                ClockCtrl.SetTimeAndDate(year, month+1, day+1, hour+1, minute+1, 0);
                ClockCtrl.Update();
                changeToPrevScreen(SCREEN_ID_MENU_MAIN);
            }
            else if (btn == objects.menu_time_ndate_button_prev)//Previous Button
            {
                changeToPrevScreen(SCREEN_ID_MENU_SETTINGS_TIME_NDATE);
            }
        }


        //Developer Menu
        if (getCurrentScreen() == SCREEN_ID_MENU_DEV)
        {
            if (btn == objects.button_back_menu_dev)//Go Back Button Clicked --- DevMenu
            {
                changeToPrevScreen(SCREEN_ID_MENU_MAIN);
            }
        }
    }
    else
    {
    }
}




#endif