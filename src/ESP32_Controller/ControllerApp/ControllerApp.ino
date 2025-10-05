/*
 Name:		ControllerApp.ino
 Created:	06.08.2025 21:41:41
 Author:	Mario
*/

//LAST WORKED ON:
//Test Temp Sensor Stuff



/*
*           TODO:
*                   SD Card Stuff ------------------------------------------------------------- NEEDS TO BE RECODED
*                   LVGL Testing -------------------------------------------------------------- DONE ------- WORKS YAY
*                   EEZ Studio Implementation --------------------------------------------------------- DONE ----- WORKS YAY
*                   FanControl --------------------- RPM Readings are still not 100% working
*                   Controller Temp Sensor ---------------------------------------------------- DONE ------ Needs Testing
*/
#include "COMMON_DEFINES.h"

#include <Arduino.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#if DISABLE_UI_AND_TOUCH != 1
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include "ui.h"
#endif

#include "SensorUpdater.h"

//#include "SdControl.h"


//SD Card Control
//SdControl SDCtrl;

//Update Task Delay Setting
static bool AllowUpdaterTask = false;

//One Wire Bus
OneWire OneWireBus(ONEWIRE_PIN);
DallasTemperature OneWireTempSensors(&OneWireBus);

//Sensor Updater
SensorUpdater SensorUpd = SensorUpdater(&OneWireTempSensors);



#if DISABLE_UI_AND_TOUCH != 1
    //Touchscreen
    SPIClass touchscreenSPI = SPIClass(VSPI);
    XPT2046_Touchscreen touchscreen(TFT_DISPLAY_TOUCH_CS_PIN);
    int touch_x, touch_y, touch_z;

    //LVGL
    #define SCREEN_WIDTH 240
    #define SCREEN_HEIGHT 320
    #define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
    uint32_t draw_buf[DRAW_BUF_SIZE / 4];

    #if LVGL_UI_TESTING == 1
    static lv_obj_t* slider_label;//LVGL TESTING
    #endif
#endif


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

    //Init SD Card
    //SDCtrl.Init(MyDisplay);
    //delay(200);
    //SDCtrl.TestWriteConfig(); //---------- TEST WRITE
    //delay(200);

    //Init GUI
#if DISABLE_UI_AND_TOUCH != 1
    InitGui();
    delay(200);
#endif

    //Create UpdaterTask
    xTaskCreate(UpdateTask, "UpdateTask", 4096, NULL, 2, NULL);
    delay(200);

    //Init Sensor Updater
    SensorUpd.Begin();
    delay(200);

    delay(200);
    Serial.println("Setup Done!");
}






//MAIN LOOP
void loop()
{
#if DISABLE_UI_AND_TOUCH != 1
    lv_task_handler();//LVGL
    #if LVGL_UI_TESTING != 1
        ui_tick();//EEZ Studio UI
    #endif
    lv_tick_inc(2);//LVGL
    delay(2);
#endif
    if (!AllowUpdaterTask && (millis() >= UPDATE_TASK_START_DELAY))
    {
        AllowUpdaterTask = true;
        SensorUpd.Start();
    }
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
        }
        delay(2);
    }
}



#if DISABLE_UI_AND_TOUCH != 1

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

    //LVGL TESTING
#if LVGL_UI_TESTING == 1
    lv_create_main_gui();
#endif

    //Init EEZ Studio UI
#if LVGL_UI_TESTING != 1
    ui_init();
    delay(200);

    //Menu Button Events
    lv_obj_add_event_cb(objects.button_open_menu, main_button_event_cb, LV_EVENT_ALL, NULL);//Open Menu Button
    lv_obj_add_event_cb(objects.button_exit_menu, main_button_event_cb, LV_EVENT_ALL, NULL);//Close Menu Button
    lv_obj_add_event_cb(objects.menu_main_opt_open_dev_menu, main_button_event_cb, LV_EVENT_ALL, NULL);//Open DevMenu Button
    lv_obj_add_event_cb(objects.button_back_menu_dev, main_button_event_cb, LV_EVENT_ALL, NULL);//Go Back Button(DevMenu)
#endif
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
#endif


















/*** LVGL TESTING ***/
#if LVGL_UI_TESTING == 1
void lv_create_main_gui(void) 
{
    // Create a text label aligned center on top ("Hello, world!")
    lv_obj_t* text_label = lv_label_create(lv_screen_active());
    lv_label_set_long_mode(text_label, LV_LABEL_LONG_WRAP);    // Breaks the long lines
    lv_label_set_text(text_label, "Hello, world!");
    lv_obj_set_width(text_label, 150);    // Set smaller width to make the lines wrap
    lv_obj_set_style_text_align(text_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(text_label, LV_ALIGN_CENTER, 0, -90);

    lv_obj_t* btn_label;
    // Create a Button (btn1)
    lv_obj_t* btn1 = lv_button_create(lv_screen_active());
    lv_obj_add_event_cb(btn1, event_handler_btn1, LV_EVENT_ALL, NULL);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -50);
    lv_obj_remove_flag(btn1, LV_OBJ_FLAG_PRESS_LOCK);

    btn_label = lv_label_create(btn1);
    lv_label_set_text(btn_label, "Button");
    lv_obj_center(btn_label);

    // Create a Toggle button (btn2)
    lv_obj_t* btn2 = lv_button_create(lv_screen_active());
    lv_obj_add_event_cb(btn2, event_handler_btn2, LV_EVENT_ALL, NULL);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 10);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_height(btn2, LV_SIZE_CONTENT);

    btn_label = lv_label_create(btn2);
    lv_label_set_text(btn_label, "Toggle");
    lv_obj_center(btn_label);

    // Create a slider aligned in the center bottom of the TFT display
    lv_obj_t* slider = lv_slider_create(lv_screen_active());
    lv_obj_align(slider, LV_ALIGN_CENTER, 0, 60);
    lv_obj_add_event_cb(slider, slider_event_callback, LV_EVENT_VALUE_CHANGED, NULL);
    lv_slider_set_range(slider, 0, 100);
    lv_obj_set_style_anim_duration(slider, 2000, 0);

    // Create a label below the slider to display the current slider value
    slider_label = lv_label_create(lv_screen_active());
    lv_label_set_text(slider_label, "0%");
    lv_obj_align_to(slider_label, slider, LV_ALIGN_BOTTOM_MID, 0, 10);
}

int btn1_count = 0;
// Callback that is triggered when btn1 is clicked
static void event_handler_btn1(lv_event_t* e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) 
    {
        btn1_count++;
        LV_LOG_USER("Button clicked %d", (int)btn1_count);
    }
}

// Callback that is triggered when btn2 is clicked/toggled
static void event_handler_btn2(lv_event_t* e) 
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = (lv_obj_t*)lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED) 
    {
        LV_UNUSED(obj);
        LV_LOG_USER("Toggled %s", lv_obj_has_state(obj, LV_STATE_CHECKED) ? "on" : "off");
    }
}


// Callback that prints the current slider value on the TFT display and Serial Monitor for debugging purposes
static void slider_event_callback(lv_event_t* e) 
{
    lv_obj_t* slider = (lv_obj_t*)lv_event_get_target(e);
    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d%%", (int)lv_slider_get_value(slider));
    lv_label_set_text(slider_label, buf);
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    LV_LOG_USER("Slider changed to %d%%", (int)lv_slider_get_value(slider));
}
#endif