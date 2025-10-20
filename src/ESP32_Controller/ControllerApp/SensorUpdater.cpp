#include "SensorUpdater.h"

//Constructor
SensorUpdater::SensorUpdater(DallasTemperature* oneWireTempSensors, SHT31* shtSensor)
{
    TempSensors = oneWireTempSensors;
    ShtSensor = shtSensor;
}


//Setup
void SensorUpdater::Begin()
{
	IsPaused = true;

    //Init Controller Door Opened Switch
    pinMode(CONTROLLER_DOOR_PIN, INPUT);

    //Init OneWire Temp Sensors
    TempSensors->begin();

    //Init SHT Sensor
    ShtSensor->begin();
}


//Start Updating
void SensorUpdater::Start()
{
	IsPaused = false;
	//Serial.println("SensorUpdater Started!");
}


//Pause Updating
void SensorUpdater::Pause()
{
	IsPaused = true;
	//Serial.println("SensorUpdater Paused!");
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
            //Serial.println("Sensor Updater: Do Update");
            

            //Update Controller Door Opened Switch
            UpdateControllerDoorOpened();

            //Update One Wire Bus Temp Sensors
            UpdateOneWireTempSensors();

            //Update SHT Temp & Humidity Sensor
            UpdateShtTempHumiditySensor();



            //DEV Print Time between Updates(sensor reading delay is currently 752ms)
            //Serial.println("Sensor Updater: TICK:    " + String((millis() - timeLastUpdate)));

            //Update last Update Time
            timeLastUpdate = millis();

            //Serial.println("Sensor Updater: Update Done");
        }
    }
}












//Controller Door Sensor
void SensorUpdater::UpdateControllerDoorOpened()
{
    int pinValue = analogRead(CONTROLLER_DOOR_PIN);
    //Serial.println("Value: " + String(pinValue));
    if ((pinValue >= 3800) && !IsControllerDoorOpened)
    {
        IsControllerDoorOpened = true;
        //Serial.println("Controller Door is now Opened!");
        #if DISABLE_UI_AND_TOUCH != true
            lv_obj_clear_flag(objects.img_door_opened, LV_OBJ_FLAG_HIDDEN);//Show Door Opened Img
        #endif
    }
    else if ((pinValue < 3700) && IsControllerDoorOpened)
    {
        IsControllerDoorOpened = false;
        //Serial.println("Controller Door is now Closed!");
        #if DISABLE_UI_AND_TOUCH != true
            lv_obj_add_flag(objects.img_door_opened, LV_OBJ_FLAG_HIDDEN);//Hide Door Opened Img
        #endif
    }
    else {}
}

bool SensorUpdater::GetControllerDoorOpened()
{
    return IsControllerDoorOpened;
}





//Controller Temp Sensor
void SensorUpdater::UpdateOneWireTempSensors()
{
    //Read Temps
    TempSensors->requestTemperatures();
    ControllerTemp = TempSensors->getTempCByIndex(0);
    if ((ControllerTemp == -127) || (ControllerTemp <= 0) && (ControllerTemp >= 100))
    {
        ControllerTemp = 0.0;
    }

    #if DISABLE_UI_AND_TOUCH != true
        if (getCurrentScreen() == SCREEN_ID_INFO_SCREEN)//If Current Screen is SCREEN_ID_INFO_SCREEN
        {
            int valueDegree = 0;
            if ((ControllerTemp != -127) && (ControllerTemp >= 0) && (ControllerTemp <= 100))
            {
                valueDegree = static_cast<int>(round(ControllerTemp));
            }
            else
            {
                ControllerTemp = 0.0;
                valueDegree = 0;
            }

            String tempStr = String(valueDegree) + "C" + DEGREE_SYMBOL_UTF8;
            strcpy(controllerTemp_label_buffer, tempStr.c_str());
            lv_label_set_text(objects.controller_temp_widget_temp_label, controllerTemp_label_buffer);
            lv_bar_set_value(objects.controller_temp_widget_temp_value, valueDegree, LV_ANIM_ON);
        }
    #endif
}

float SensorUpdater::GetControllerTemp()
{
    return ControllerTemp;
}






//SHT Temp & Humidity Sensor
void SensorUpdater::UpdateShtTempHumiditySensor()
{
    if (ShtSensor->isConnected())
    {
        //Update Readings
        ShtSensor->read();
        TerrariumTemp = ShtSensor->getTemperature();
        TerrariumHumidity = ShtSensor->getHumidity();
        //Serial.println("Temp: " + String(TerrariumTemp));
        //Serial.println("Humid: " + String(TerrariumHumidity));

        if ((TerrariumTemp == -45) || (TerrariumTemp <= 0) || (TerrariumTemp >= 100))
        {
            TerrariumTemp = 0.0;
        }
    }
    else
    {
        TerrariumTemp = 0.0;
    }


    #if DISABLE_UI_AND_TOUCH != true
        if (getCurrentScreen() == SCREEN_ID_INFO_SCREEN)//If Current Screen is SCREEN_ID_INFO_SCREEN
        {
            //Temp
            int valueDegree = 0;
            if ((TerrariumTemp != -45) && (TerrariumTemp >= 0) && (TerrariumTemp <= 100))
            {
                valueDegree = static_cast<int>(round(TerrariumTemp));
            }
            else
            {
                TerrariumTemp = 0.0;
                valueDegree = 0;
            }
            String tempStr = String(valueDegree) + "C" + DEGREE_SYMBOL_UTF8;
            strcpy(terrariumTemp_label_buffer, tempStr.c_str());
            lv_label_set_text(objects.terrarium_air_temp_value, terrariumTemp_label_buffer);
            lv_bar_set_value(objects.terrarium_air_temp_bar, valueDegree, LV_ANIM_ON);


            //Humidity
            int valueHum = 0;
            if ((TerrariumHumidity >= 0) && (TerrariumHumidity <= 100))
            {
                valueHum = static_cast<int>(round(TerrariumHumidity));
            }
            else
            {
                TerrariumHumidity = 0.0;
                valueHum = 0;
            }
            String humidStr = String(valueHum) + "%";
            strcpy(terrariumHumidity_label_buffer, humidStr.c_str());
            lv_label_set_text(objects.terrarium_humidity_value, terrariumHumidity_label_buffer);
            lv_bar_set_value(objects.terrarium_humidity_bar, valueHum, LV_ANIM_ON);
        }
    #endif
}

float SensorUpdater::GetTerrariumTemp()
{
    return TerrariumTemp;
}

float SensorUpdater::GetTerrariumHumidity()
{
    return TerrariumHumidity;
}