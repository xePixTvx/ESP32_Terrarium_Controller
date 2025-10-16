/*
 Name:		ControllerApp.ino
 Created:	06.08.2025 21:41:41
 Author:	Mario
*/

//LAST WORKED ON: RTC Testing



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
*                   ESP_NOW                                 -------------------------------- NOT STARTED
*/
#include "COMMON_DEFINES.h"

#include <Arduino.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SHT31.h>



#include "SensorUpdater.h"
#include "ClockControl.h"


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

    if (ClockCtrl.IsGuiUpdateNeeded())
    {
        Serial.println("Clock Gui Update");
        ClockCtrl.GuiUpdateDone();
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


