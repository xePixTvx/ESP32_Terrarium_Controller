#include "ClockControl.h"

/*
DateTime now = rtc.now();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    Serial.println();
    delay(3000);
*/





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

    //Force Reset Time & Date
    //SetTimeAndDate(2000, 1, 1, 20, 15, 0);//01.01.2000 @ 20:15:00
    //RTC_Device.adjust(DateTime(F(__DATE__), F(__TIME__)));//Set to Compile DateTime
}


void ClockControl::SetTimeAndDate(int year, int month, int day, int hour, int minute, int second)
{
    Serial.println("ReAdjusted RTC Date & Time!");
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

    if ((SecondsSinceLastUpdate >= 60) && !GuiNeedsUpdate)
    {
        GuiNeedsUpdate = true;
    }


    //TestClock();
}


DateTime ClockControl::GetDateTime()
{
    return RTC_Device.now();
}


bool ClockControl::IsGuiUpdateNeeded()
{
    if (GuiNeedsUpdate)
    {
        return true;
    }
    return false;
}

void ClockControl::GuiUpdateDone()
{
    GuiNeedsUpdate = false;
    SecondsSinceLastUpdate = 0;
}




/*void ClockControl::TestClock()
{
    DateTime now = GetDateTime();

    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(weekDayNames[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    Serial.println();
    delay(1000);
}*/