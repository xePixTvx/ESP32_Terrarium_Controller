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

class SensorUpdater
{

	public:
		SensorUpdater(DallasTemperature*);
		void Begin();
		void Start();
		void Pause();
		void Update();
		bool GetPausedStatus();


		//Controller Door Opened
		bool GetControllerDoorOpened();

		//Controller Temp
		float GetControllerTemp();


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
};