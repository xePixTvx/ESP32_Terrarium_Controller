#include "ClockControl.h"

//Constructor
ClockControl::ClockControl()
{
}


void ClockControl::Begin()
{
    if (!RTC_Device.begin())
    {
        Serial.println("RTC Init Failed!");
        Serial.flush();
        while (1)
        {
            delay(10);
        }
    }

    if (RTC_Device.lostPower())
    {
        SetTimeAndDate(2000, 1, 1, 20, 15, 0);//01.01.2000 @ 20:15:00
    }
}


void ClockControl::SetTimeAndDate(int year, int month, int day, int hour, int minute, int second)
{
    //Serial.println("ReAdjusted RTC Date & Time!");
    RTC_Device.adjust(DateTime(year, month, day, hour, minute, second));
}


void ClockControl::Update()
{
    DateTime now = GetDateTime();

    if (SecondsAtLastUpdate != now.second())
    {
        SecondsAtLastUpdate = now.second();
        SecondsSinceLastUpdate++;
        //Serial.println("Seconds Since Last Update: " + String(SecondsSinceLastUpdate));
    }

    if ((SecondsSinceLastUpdate >= 60))
    {
        UpdateClockGui(now);
        SecondsSinceLastUpdate = 0;
    }
}




void ClockControl::UpdateClockGui(DateTime now)
{
    //Day Name
    String temp_dayName = weekDayNames[now.dayOfTheWeek()];
    strcpy(dayLabelBuffer, temp_dayName.c_str());

    //Date
    String temp_date = ConvertNumberToGuiString(now.day()) + "." + ConvertNumberToGuiString(now.month()) + "." + String(now.year());
    strcpy(dateLabelBuffer, temp_date.c_str());

    //Time
    String temp_time = ConvertNumberToGuiString(now.hour()) + ":" + ConvertNumberToGuiString(now.minute());
    strcpy(timeLabelBuffer, temp_time.c_str());

    #if DISABLE_UI_AND_TOUCH != true
        if (getCurrentScreen() == SCREEN_ID_INFO_SCREEN)//If Current Screen is SCREEN_ID_INFO_SCREEN
        {
            //GUI Update Day Name
            lv_label_set_text(objects.clock_day, dayLabelBuffer);

            //GUI Update Date
            lv_label_set_text(objects.clock_date, dateLabelBuffer);

            //GUI Update Time
            lv_label_set_text(objects.clock_time, timeLabelBuffer);
        }
    #endif
}


DateTime ClockControl::GetDateTime()
{
    return RTC_Device.now();
}


String ClockControl::ConvertNumberToGuiString(uint8_t number)
{
    String numberToReturn = "00";
    if (number <= 9)
    {
        numberToReturn = "0" + String(number);
    }
    else
    {
        numberToReturn = String(number);
    }
    return numberToReturn;
}