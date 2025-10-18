#include "COMMON_DEFINES.h"

#include <Arduino.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SHT31.h>

#include <lvgl.h>
#include "ui.h"

class SensorUpdater
{

	public:
		SensorUpdater(DallasTemperature*, SHT31*);
		void Begin();
		void Start();
		void Pause();
		void Update();
		bool GetPausedStatus();


		//Controller Door Opened
		bool GetControllerDoorOpened();

		//Controller Temp
		float GetControllerTemp();

		//SHT Temp & Humidity Sensor
		float GetTerrariumTemp();
		float GetTerrariumHumidity();


	private:
		bool IsPaused = false;
		unsigned long timeLastUpdate = 0;

		const String DEGREE_SYMBOL_UTF8 = "\xc2\xb0";

		//Controller Door Opened
		bool IsControllerDoorOpened = false;
		void UpdateControllerDoorOpened();

		//Controller Temp
		DallasTemperature* TempSensors;
		char controllerTemp_label_buffer[6];
		float ControllerTemp = 0.0;
		void UpdateOneWireTempSensors();

		//SHT Temp & Humidity Sensor
		SHT31* ShtSensor;
		char terrariumTemp_label_buffer[6];
		char terrariumHumidity_label_buffer[6];
		float TerrariumTemp = 0.0;
		float TerrariumHumidity = 0.0;
		void UpdateShtTempHumiditySensor();
};