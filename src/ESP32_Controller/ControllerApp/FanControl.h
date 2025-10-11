#include "COMMON_DEFINES.h"
#include <Arduino.h>
#include <FunctionalInterrupt.h>


class FanControl
{
	public:
		FanControl();
		void Begin();
		void Update();
		void SetSpeedPercent(int fan, int percent);
		int GetSpeedPercent(int fan);
		int GetRpm(int fan);


	private:
		//Fan 1
		int Fan1_Speed_Percent = 0;
		volatile unsigned long Fan1_RPM_PulseCount = 0;
		unsigned long Fan1_RPM_LastRpmTime = 0;
		void Fan1_RpmISR();
		int Fan1_RPM = 0;

		//Fan 2
		int Fan2_Speed_Percent = 0;
		volatile unsigned long Fan2_RPM_PulseCount = 0;
		unsigned long Fan2_RPM_LastRpmTime = 0;
		void Fan2_RpmISR();
		int Fan2_RPM = 0;

		//Utils
		int ConvertPercentTo8Bit(int percent);
};