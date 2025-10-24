#include "ConfigControl.h"


//Constructor
ConfigControl::ConfigControl()
{
}


//Begin
void ConfigControl::Begin()
{
	if (!LittleFS.begin(LITTLEFS_FORMAT_ON_FAIL))
	{
		Serial.println("LittleFS Failed!");
		Serial.println("Formating Now!");
		delay(200);
	}

    //Create Root Directory if not already existing
    if (!directoryExists(LittleFS, "/data"))
    {
        createDirectory(LittleFS, "/data");
    }
    delay(20);

    //Create Config File if not already existing
    if (!fileExists(LittleFS, "/data/config.cfg"))
    {
        writeFile(LittleFS, "/data/config.cfg", "");
    }
    /*else//Write stuff for testing
    {
        writeFile(LittleFS, "/data/config.cfg", "Hello_World_Test_Content");
    }*/
    delay(20);

    //listDirectories(LittleFS, "/", 2);//TEST
    //delay(200);
    //readFile(LittleFS, "/data/config.cfg");//TEST
}






/***************************************** Some Common Utils *****************************************/
void ConfigControl::listDirectories(fs::FS& fs, const char* dirname, uint8_t levels)
{
    File root = fs.open(dirname);
    if (!root) 
    {
        Serial.println("- failed to open directory");
        return;
    }
    if (!root.isDirectory()) 
    {
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) 
    {
        if (file.isDirectory()) 
        {
            Serial.print("/");
            Serial.println(file.name());
            if (levels) 
            {
                listDirectories(fs, file.path(), levels - 1);
            }
        }
        else 
        {
            Serial.print("FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void ConfigControl::createDirectory(fs::FS& fs, const char* path)
{
	Serial.printf("Creating Dir: %s\n", path);
	if (fs.mkdir(path)) 
	{
		Serial.println("Dir created");
	}
	else 
	{
		Serial.println("mkdir failed");
	}
}

bool ConfigControl::directoryExists(fs::FS& fs, const char* dirname)
{
    File root = fs.open(dirname);
    if (!root || !root.isDirectory())
    {
        return false;
    }
    root.close();
    return true;
}

void ConfigControl::writeFile(fs::FS& fs, const char* filepath, const char* content)
{
    File file = fs.open(filepath, FILE_WRITE);
    if (!file) 
    {
        Serial.println("Failed to open Config File!");
        return;
    }
    if (file.print(content))
    {
        Serial.println("Config File written!");
    }
    else 
    {
        Serial.println("Config File writing failed!");
    }
    file.close();
}

void ConfigControl::readFile(fs::FS& fs, const char* filepath)////////TEST MEEEEEEEEEE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
{
    File file = fs.open(filepath);
    if (!file || file.isDirectory()) 
    {
        Serial.println("Failed to read Config File!");
        return;
    }

    Serial.println("- read from file:");
    while (file.available()) 
    {
        Serial.write(file.read());
    }
    file.close();
}

bool ConfigControl::fileExists(fs::FS& fs, const char* filepath)
{
    File file = fs.open(filepath, "r", false);
    if (!file || file.isDirectory())
    {
        return false;
    }
    file.close();
    return true;
}
/***************************************** Some Common Utils END *****************************************/