#include "LightHeaterControl.h"


//Constructor
LightHeaterControl::LightHeaterControl()
{
}




void LightHeaterControl::Begin()
{
	//Set pinModes
	pinMode(LIGHT_RELAIS_SIGNAL_PIN, OUTPUT);
	pinMode(HEATER_RELAIS_SIGNAL_PIN, OUTPUT);

	//Turn Light & Heater Off
	SetLight(false);
	SetHeater(false);
}




void LightHeaterControl::SetLight(bool on)
{
	if (on)
	{
		digitalWrite(LIGHT_RELAIS_SIGNAL_PIN, 1);
		LightOn = true;
		Serial.println("Light: ON");
	}
	else
	{
		digitalWrite(LIGHT_RELAIS_SIGNAL_PIN, 0);
		LightOn = false;
		Serial.println("Light: OFF");
	}
}

void LightHeaterControl::SetHeater(bool on)
{
	if (on)
	{
		digitalWrite(HEATER_RELAIS_SIGNAL_PIN, 1);
		HeaterOn = true;
		Serial.println("Heater: ON");
	}
	else
	{
		digitalWrite(HEATER_RELAIS_SIGNAL_PIN, 0);
		HeaterOn = false;
		Serial.println("Heater: OFF");
	}
}


bool LightHeaterControl::IsLightOn()
{
	if (LightOn && (digitalRead(LIGHT_RELAIS_SIGNAL_PIN) == HIGH))
	{
		return true;
	}
	return false;
}

bool LightHeaterControl::IsHeaterOn()
{
	if (HeaterOn && (digitalRead(HEATER_RELAIS_SIGNAL_PIN) == HIGH))
	{
		return true;
	}
	return false;
}