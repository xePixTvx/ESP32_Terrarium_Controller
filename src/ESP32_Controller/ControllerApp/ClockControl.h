#include "COMMON_DEFINES.h"

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SPIDevice.h>
#include <Adafruit_I2CRegister.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GenericDevice.h>
#include <Adafruit_BusIO_Register.h>
#include <RTClib.h>


#include <lvgl.h>
#include "ui.h"



class ClockControl
{
	public:
		ClockControl();
		void Begin();
		void SetTimeAndDate(int year, int month, int day, int hour, int minute, int second);
		void Update();
		DateTime GetDateTime();


	private:
		char weekDayNames[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
		String ConvertNumberToGuiString(uint8_t number);
		RTC_DS3231 RTC_Device;
		uint8_t SecondsAtLastUpdate = 0;
		uint8_t SecondsSinceLastUpdate = 60;
		char dayLabelBuffer[10];
		char dateLabelBuffer[15];
		char timeLabelBuffer[6];
		void UpdateClockGui(DateTime now);
};