#include "COMMON_DEFINES.h"

#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>


class ConfigControl
{
	public:
		ConfigControl();
		void Begin();





	private:
		//Some Common Utils
		void listDirectories(fs::FS& fs, const char* dirname, uint8_t levels);
		void createDirectory(fs::FS& fs, const char* path);
		bool directoryExists(fs::FS& fs, const char* dirname);
		void writeFile(fs::FS& fs, const char* filepath, const char* content);
		void readFile(fs::FS& fs, const char* filepath);
		bool fileExists(fs::FS& fs, const char* filepath);



};