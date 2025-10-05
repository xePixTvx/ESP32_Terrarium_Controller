#include "COMMON_DEFINES.h"
#include <Arduino.h>
#include <SD.h>
#include <TFT_eSPI.h>

class SdControl
{
	public:
		void Init(TFT_eSPI &MyDisplay);
		void TestWriteConfig();




	private:
		bool SDCard_Initialized;
		File MyFile;
};