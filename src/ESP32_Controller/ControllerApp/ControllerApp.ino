/*
 Name:		ControllerApp.ino
 Created:	06.08.2025 21:41:41
 Author:	Mario
*/

//LAST WORKED ON: Combining shit again



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
*                   RTC(DS3231)                             -------------------------------- WIP
*                   ESP-NOW                                 -------------------------------- NOPE LETS NOT DO IT
*/
#include "COMMON_DEFINES.h"

#include <Arduino.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SHT31.h>
//#include <esp_now.h>
//#include <WiFiMulti.h>


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



/*
//ESP-NOW
uint8_t DisplayControllerMacAddress[] = { 0x78, 0x1C, 0x3C, 0x2C, 0x71, 0xCC };
esp_now_peer_info_t peerInfo;

//ESP-NOW Message Structure
typedef struct struct_EspNowMessage
{
    String msg;
} struct_EspNowMessage;

//ESP-NOW Incoming Message
struct_EspNowMessage IncomingEspNowMessage;


//ESP-NOW MessageToSend
struct_EspNowMessage MessageToSend;
*/


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

    //Init ESP-NOW
    //InitEspNow();
    //delay(200);
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

    if (ClockCtrl.IsGuiUpdateNeeded())
    {
        Serial.println("Clock Gui Update");
        ClockCtrl.GuiUpdateDone();
    }





#if DISABLE_UI_AND_TOUCH != true
    lv_task_handler();//LVGL
    ui_tick();//EEZ Studio UI
    lv_tick_inc(2);//LVGL
    delay(2);
#endif





    /*MessageToSend.msg = "Hello World!";

    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(DisplayControllerMacAddress, (uint8_t*)&MessageToSend, sizeof(MessageToSend));
    if (result == ESP_OK) 
    {
        Serial.println("Sent with success");
    }
    else 
    {
        Serial.println("Error sending the data");
    }
    delay(5000);*/

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

#endif










/*
void InitEspNow()
{
    //Set as Wi-Fi Station
    WiFi.mode(WIFI_STA);

    //Init ESP-NOW
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW Init ERROR!");
        return;
    }

    //On Data Sent
    esp_now_register_send_cb(esp_now_send_cb_t(OnDataSent));

    //Register peer
    memcpy(peerInfo.peer_addr, DisplayControllerMacAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    //Add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("ESP-NOW: Failed to add peer");
        return;
    }

    //On Data Received
    esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

    Serial.println("ESP-NOW Initialized!");
    delay(200);
}


// Callback when data is sent
static void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status) 
{
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    if (status == 0) 
    {
        Serial.println("Delivery Success");
    }
    else 
    {
        Serial.println("Delivery Fail");
    }
}

// Callback when data is received
void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) 
{
    memcpy(&IncomingEspNowMessage, incomingData, sizeof(IncomingEspNowMessage));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.println("MSG: " + IncomingEspNowMessage.msg);
}
*/


