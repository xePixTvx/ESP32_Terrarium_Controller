#include "COMMON_DEFINES.h"

#include <Arduino.h>
#include <Wire.h>


class LightHeaterControl
{
	public:
		LightHeaterControl();
		void Begin();
		void SetLight(bool on);
		void SetHeater(bool on);
		bool IsLightOn();
		bool IsHeaterOn();

	private:
		bool LightOn = false;
		bool HeaterOn = false;
};