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
        writeFile(LittleFS, "/data/config.cfg", "{LightOnTime=9:15|LightOffTime=23:15}");
    }
    /*else//Write stuff for testing
    {
        writeFile(LittleFS, "/data/config.cfg", "{LightOnTime=20:15|LightOffTime=23:15|Test3=false|Test4=true}");
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

String ConfigControl::readFile(fs::FS& fs, const char* filepath)
{
    String alreadyReadChars = "";
    char currentChar;

    File file = fs.open(filepath);
    if (!file || file.isDirectory()) 
    {
        Serial.println("Failed to read Config File!");
        return alreadyReadChars;
    }

    while (file.available()) 
    {
        currentChar = file.read();
        alreadyReadChars += currentChar;
    }
    //Serial.println(alreadyReadChars);
    file.close();

    return alreadyReadChars;
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



//Load Config File
void ConfigControl::LoadConfig()
{
    String contents = readFile(LittleFS, "/data/config.cfg");
    //Serial.println("FileContent: " + contents);

    //If File content start with '{' and ends with '}'
    if (contents.startsWith("{") && contents.endsWith("}"))
    {
        //Remove '{' and '}'
        String contentsRemovedBeginEndIndicators = removeChar(contents, '{');
        contentsRemovedBeginEndIndicators = removeChar(contentsRemovedBeginEndIndicators, '}');

        //Get Settings Lines
        std::vector<String> settingLines = getSettingLines(contentsRemovedBeginEndIndicators);


        //{LightOnTime=9:15|LightOffTime=23:15}
        for (int i = 0; i < settingLines.size(); i++)
        {
            if (settingLines[i].startsWith("LightOnTime"))
            {
                String lightOnTimeSetting = getSetting(settingLines[i]);
                int hour = get_hour_minute(lightOnTimeSetting, false);
                int minute = get_hour_minute(lightOnTimeSetting, true);
                SetSetting_LightOnTime(hour, minute);
            }
            else if (settingLines[i].startsWith("LightOffTime"))
            {
                String lightOffTimeSetting = getSetting(settingLines[i]);
                int hour = get_hour_minute(lightOffTimeSetting, false);
                int minute = get_hour_minute(lightOffTimeSetting, true);
                SetSetting_LightOffTime(hour, minute);
            }

            //Serial.println("Line: " + settingLines[i]);
            delay(50);
        }
    }
    else
    {
        Serial.println("Cannot read the config file!");
        Serial.println("Using Defaults instead!");
        //Use default settings instead
        SetSetting_LightOnTime(9, 15);
        SetSetting_LightOffTime(23, 15);
    }

    //Serial.println("Contents: " + String(contentsRemovedBeginEndIndicators.length()) + " ---- " + contentsRemovedBeginEndIndicators);
    delay(200);
    Serial.println("Config Loaded!");
}



void ConfigControl::SaveConfig()
{
    String strToSave = "{";

    DateTime lightOnTime = GetSetting_LightOnTime();
    DateTime lightOffTime = GetSetting_LightOffTime();
    strToSave += "LightOnTime=" + String(lightOnTime.hour()) + ":" + String(lightOnTime.minute()) + "|";
    strToSave += "LightOffTime=" + String(lightOffTime.hour()) + ":" + String(lightOffTime.minute());


    strToSave += "}";

    //Serial.println("To Save: " + strToSave);
    writeFile(LittleFS, "/data/config.cfg", strToSave.c_str());
}






//Remove a char from a string
String ConfigControl::removeChar(String str, const char charToRemove)
{
    //String to Return
    String strToReturn = "";

    //Convert to Char Array
    int charSize = str.length() + 1;
    char charBuf[charSize];
    str.toCharArray(charBuf, charSize);

    //Remove Char
    for (int i = 0; i < charSize; i++)
    {
        if (charBuf[i] != charToRemove)
        {
            strToReturn += charBuf[i];
        }
    }

    //Return new string
    return strToReturn;
}


//Get a vector<string> with all the Setting Lines
std::vector<String> ConfigControl::getSettingLines(String str)
{
    //Convert to Char Array
    int charSize = str.length() + 1;
    char charBuf[charSize];
    str.toCharArray(charBuf, charSize);


    int settingsSize = 0;
    int currentSetting = 0;
    String currentSettingStr[20];
    std::vector<String> currentSettingLines;

    //Split settings
    for (int i = 0; i < charSize; i++)
    {
        if (charBuf[i] == '|')
        {
            currentSetting++;
            settingsSize++;
        }
        else
        {
            currentSettingStr[currentSetting] += charBuf[i];
        }
    }

    //Add Setting Lines to vector
    for (int i = 0; i < settingsSize + 1; i++)
    {
        currentSettingLines.push_back(currentSettingStr[i]);
    }

    return currentSettingLines;
}


//Get the Setting itself from a string
String ConfigControl::getSetting(String str)
{
    //Convert to Char Array
    int charSize = str.length() + 1;
    char charBuf[charSize];
    str.toCharArray(charBuf, charSize);

    //Read Setting(after '=')
    String setting = "";
    bool readSetting = false;
    for (int i = 0; i < charSize; i++)
    {
        if (readSetting)
        {
            setting += charBuf[i];
        }
        if (charBuf[i] == '=')
        {
            readSetting = true;
        }
    }
    return setting;
}






//SETTINGS

void ConfigControl::SetSetting_LightOnTime(int hour, int minute)
{
    Setting_LightOnTime = DateTime(0, 0, 0, hour, minute, 0);
    //Serial.println("LightOnTime set to: " + String(hour) + ":" + String(minute));
}

DateTime ConfigControl::GetSetting_LightOnTime()
{
    return Setting_LightOnTime;
}


void ConfigControl::SetSetting_LightOffTime(int hour, int minute)
{
    Setting_LightOffTime = DateTime(0, 0, 0, hour, minute, 0);
    //Serial.println("LightOffTime set to: " + String(hour) + ":" + String(minute));
}

DateTime ConfigControl::GetSetting_LightOffTime()
{
    return Setting_LightOffTime;
}


int ConfigControl::get_hour_minute(String str, bool getMinute = false)
{
    //Convert to Char Array
    int charSize = str.length() + 1;
    char charBuf[charSize];
    str.toCharArray(charBuf, charSize);

    String time = "";
    for (int i = 0; i < charSize; i++)
    {
        if (!getMinute)
        {
            if (charBuf[i] == ':')
            {
                break;
            }
            time += charBuf[i];
        }
        else
        {
            if (charBuf[i] == ':')
            {
                time.clear();
                continue;
            }
            time += charBuf[i];
        }
    }
    return atoi(time.c_str());
}
