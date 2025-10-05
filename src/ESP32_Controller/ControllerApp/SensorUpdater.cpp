#include "SensorUpdater.h"

//Constructor
SensorUpdater::SensorUpdater(DallasTemperature* oneWireTempSensors)
{
    TempSensors = oneWireTempSensors;
}


//Setup
void SensorUpdater::Begin()
{
	IsPaused = true;

    //Init Controller Door Opened Switch
    pinMode(CONTROLLER_DOOR_PIN, INPUT);

    //Init OneWire Temp Sensors
    TempSensors->begin();
}


//Start Updating
void SensorUpdater::Start()
{
	IsPaused = false;
	Serial.println("SensorUpdater Started!");
}


//Pause Updating
void SensorUpdater::Pause()
{
	IsPaused = true;
	Serial.println("SensorUpdater Paused!");
}


//Get Paused Status
bool SensorUpdater::GetPausedStatus()
{
    return IsPaused;
}














//Update Sensor Task LOOP
void SensorUpdater::Update()
{
    if (!GetPausedStatus())
    {
        if ((millis() - timeLastUpdate) >= UPDATE_SENSORS_DELAY)
        {
            //Update Controller Door Opened Switch
            UpdateControllerDoorOpened();

            //Update One Wire Bus Temp Sensors
            UpdateOneWireTempSensors();



            //DEV Print Time between Updates
            //Serial.println("Sensor Updater: TICK:    " + String((millis() - timeLastUpdate)));

            //Update last Update Time
            timeLastUpdate = millis();
        }
    }
}













void SensorUpdater::UpdateControllerDoorOpened()
{
    int pinValue = analogRead(CONTROLLER_DOOR_PIN);
    //Serial.println("Value: " + String(pinValue));
    if ((pinValue >= 3800) && !IsControllerDoorOpened)
    {
        IsControllerDoorOpened = true;
        //Serial.println("Controller Door is now Opened!");
        #if DISABLE_UI_AND_TOUCH != 1
            #if LVGL_UI_TESTING != 1
                lv_obj_clear_flag(objects.img_door_opened, LV_OBJ_FLAG_HIDDEN);//Show Door Opened Img
            #endif
        #endif
    }
    else if ((pinValue < 3700) && IsControllerDoorOpened)
    {
        IsControllerDoorOpened = false;
        //Serial.println("Controller Door is now Closed!");
        #if DISABLE_UI_AND_TOUCH != 1
            #if LVGL_UI_TESTING != 1
                lv_obj_add_flag(objects.img_door_opened, LV_OBJ_FLAG_HIDDEN);//Hide Door Opened Img
            #endif
        #endif
    }
    else {}
}

bool SensorUpdater::GetControllerDoorOpened()
{
    return IsControllerDoorOpened;
}






void SensorUpdater::UpdateOneWireTempSensors()
{
    //Read Temps
    TempSensors->requestTemperatures();
    ControllerTemp = TempSensors->getTempCByIndex(0);

    #if DISABLE_UI_AND_TOUCH != 1
        #if LVGL_UI_TESTING != 1
            if (getCurrentScreen() == SCREEN_ID_INFO_SCREEN)//If Current Screen is SCREEN_ID_INFO_SCREEN
            {
                if ((ControllerTemp != -127) && (ControllerTemp >= 0) && (ControllerTemp <= 100))
                {
                    //BAR = objects.controller_temp_widget_temp_value

                    //LVGL Labels info: https://docs.lvgl.io/master/details/widgets/label.html

                    String tempStr = String(ControllerTemp) + "C" + DEGREE_SYMBOL_UTF8;
                    strcpy(controllerTemp_label_buffer, tempStr.c_str());
                    lv_label_set_text(objects.controller_temp_widget_temp_label, controllerTemp_label_buffer);
                    lv_bar_set_value(objects.controller_temp_widget_temp_value, 80, LV_ANIM_ON);
                }
                else
                {
                    String tempStr = "0C" + DEGREE_SYMBOL_UTF8;
                    strcpy(controllerTemp_label_buffer, tempStr.c_str());
                    lv_label_set_text(objects.controller_temp_widget_temp_label, controllerTemp_label_buffer);
                    lv_bar_set_value(objects.controller_temp_widget_temp_value, 0, LV_ANIM_ON);
                }
            }
        #endif
    #endif
}

float SensorUpdater::GetControllerTemp()
{
    return ControllerTemp;
}