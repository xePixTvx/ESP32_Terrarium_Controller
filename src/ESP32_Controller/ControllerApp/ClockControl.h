#include "COMMON_DEFINES.h"

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GenericDevice.h>
#include <Adafruit_BusIO_Register.h>
#include <RTClib.h>



class ClockControl
{
	public:
		ClockControl();
		void Begin();
		void SetTimeAndDate(int year, int month, int day, int hour, int minute, int second);
		void Update();
		DateTime GetDateTime();
		bool IsGuiUpdateNeeded();
		void GuiUpdateDone();


	private:
		char weekDayNames[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
		RTC_DS3231 RTC_Device;
		uint8_t SecondsAtLastUpdate = 0;
		uint8_t SecondsSinceLastUpdate = 0;
		bool GuiNeedsUpdate = false;
};