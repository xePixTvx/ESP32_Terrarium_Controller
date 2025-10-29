#include "COMMON_DEFINES.h"

#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <RTClib.h>


class ConfigControl
{
	public:
		ConfigControl();
		void Begin();
		void LoadConfig();
		void SaveConfig();
		void SetSetting_LightOnTime(int hour, int minute);
		DateTime GetSetting_LightOnTime();
		void SetSetting_LightOffTime(int hour, int minute);
		DateTime GetSetting_LightOffTime();




	private:
		//Some Common Utils
		void listDirectories(fs::FS& fs, const char* dirname, uint8_t levels);
		void createDirectory(fs::FS& fs, const char* path);
		bool directoryExists(fs::FS& fs, const char* dirname);
		void writeFile(fs::FS& fs, const char* filepath, const char* content);
		String readFile(fs::FS& fs, const char* filepath);
		bool fileExists(fs::FS& fs, const char* filepath);

		//Config File reading
		String removeChar(String str, const char charToRemove);
		std::vector<String> getSettingLines(String str);
		String getSetting(String str);

		//Settings
		DateTime Setting_LightOnTime;
		DateTime Setting_LightOffTime;
		int get_hour_minute(String str, bool getMinute);

};