#include "SdControl.h"

void SdControl::Init(TFT_eSPI& MyDisplay)
{
    SDCard_Initialized = false;

    Serial.println("----- Initialize SD CARD -----");
    delay(200);
    if (!SD.begin(SD_CARD_CS_PIN, MyDisplay.getSPIinstance()))
    {
        Serial.println("SD Card not found!");
        delay(200);
        Serial.println("----- SD Card Initialization Failed -----");
        SDCard_Initialized = false;
        return;
    }
    delay(500);

    //Print SD Card Type
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE)
    {
        Serial.println("SD Card Type is not readable!");
        delay(200);
        Serial.println("----- SD Card Initialization Failed -----");
        SDCard_Initialized = false;
        return;
    }
    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC)
    {
        Serial.println("MMC");
    }
    else if (cardType == CARD_SD)
    {
        Serial.println("SDSC");
    }
    else if (cardType == CARD_SDHC)
    {
        Serial.println("SDHC");
    }
    else
    {
        Serial.println("UNKNOWN");
    }
    delay(200);

    //Print SD Card Size
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    SDCard_Initialized = true;
    Serial.println("----- SD Card Initialized -----");
    delay(500);
}


void SdControl::TestWriteConfig()
{
    Serial.println("----- Write Test Config -----");
    delay(200);

    if (SDCard_Initialized)
    {
        //Create/Write TEST FILE
        MyFile = SD.open("/test.txt", FILE_WRITE);

        // if the file opened okay, write to it:
        if (SD.exists("/test.txt"))
        {
            Serial.println("Test Writing to test.txt...");
            delay(200);
            MyFile.println("Test Line");
            MyFile.println("Next Test Line YAY");
            delay(500);
            // close the file:
            MyFile.close();
            delay(200);
            Serial.println("done.");
            delay(200);
        }
        else
        {
            // if the file didn't open, print an error:
            Serial.println("test.txt doesnt exist!");
            delay(200);
            Serial.println("----- Test Config Write FAILED -----");
            return;
        }

        delay(500);
        Serial.println("----- Test Config Written -----");
        delay(200);
    }
    else
    {
        Serial.println("Cannot write Test Config: SD Card not Initialized!");
        Serial.println("----- Test Config Written -----");
        delay(200);
    }
}