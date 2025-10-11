#include "FanControl.h"


//Constructor
FanControl::FanControl()
{
}

//Init
void FanControl::Begin()
{
    //FAN1
    Fan1_Speed_Percent = 0;
    ledcAttach(FAN1_PWM_PIN, FAN_PWM_FREQ, FAN_PWM_RESOLUTION);//Configure PWM Fan1
    SetSpeedPercent(0, 0);

    pinMode(FAN1_RPM_PIN, INPUT_PULLUP);//Configure RPM Fan1
    attachInterrupt(digitalPinToInterrupt(FAN1_RPM_PIN), std::bind(&FanControl::Fan1_RpmISR, this), FALLING);


    //FAN2
    Fan2_Speed_Percent = 0;
    ledcAttach(FAN2_PWM_PIN, FAN_PWM_FREQ, FAN_PWM_RESOLUTION);//Configure PWM Fan2
    SetSpeedPercent(1, 0);

    pinMode(FAN2_RPM_PIN, INPUT_PULLUP);//Configure RPM Fan2
    attachInterrupt(digitalPinToInterrupt(FAN2_RPM_PIN), std::bind(&FanControl::Fan2_RpmISR, this), FALLING);


    //Timing
    Fan1_RPM_LastRpmTime = millis();
    Fan1_RPM_LastRpmTime = millis();

    delay(50);
}




//Update
void FanControl::Update()
{
    //Serial.println("Fan1 Current Speed(%): " + String(ConvertPercentTo8Bit(GetSpeedPercent(0))));
    //Serial.println("Fan2 Current Speed(%): " + String(GetSpeedPercent(1)));

    //Current Time
    unsigned long currentTime = millis();

    // Calculate RPM every second for Fan1
    if ((currentTime - Fan1_RPM_LastRpmTime) >= FAN_RPM_SAMPLE_TIME)
    {
        // Calculate RPM (2 pulses per revolution for most fans)
        unsigned long rpm1 = (Fan1_RPM_PulseCount * 60000) / (FAN_RPM_SAMPLE_TIME * 2);

        // Estimate target RPM based on fan speed (values for Noctua NF-A4x10 5V PWM)
        // This fan has max RPM of 5000, adjust for your specific fan
        unsigned long targetRPM1 = map(GetSpeedPercent(0), 0, 100, 0, FAN_MAX_RPM);

        Fan1_RPM = rpm1;
        //Serial.println("Fan1 Target RPM: " + String(targetRPM));
        //Serial.println("Fan1 RPM: " + String(rpm));
        //delay(100);

        // Reset counters
        Fan1_RPM_PulseCount = 0;
        Fan1_RPM_LastRpmTime = currentTime;
    }

    // Calculate RPM every second for Fan2
    if ((currentTime - Fan2_RPM_LastRpmTime) >= FAN_RPM_SAMPLE_TIME)
    {
        // Calculate RPM (2 pulses per revolution for most fans)
        unsigned long rpm2 = (Fan2_RPM_PulseCount * 60000) / (FAN_RPM_SAMPLE_TIME * 2);

        // Estimate target RPM based on fan speed (values for Noctua NF-A4x10 5V PWM)
        // This fan has max RPM of 5000, adjust for your specific fan
        unsigned long targetRPM2 = map(GetSpeedPercent(1), 0, 100, 0, FAN_MAX_RPM);

        Fan2_RPM = rpm2;
        //Serial.println("Fan2 Target RPM: " + String(targetRPM2));
        //Serial.println("Fan2 RPM: " + String(rpm2));
        //delay(100);

        // Reset counters
        Fan2_RPM_PulseCount = 0;
        Fan2_RPM_LastRpmTime = currentTime;
    }
}



//Set Fan Speed in Percent
void FanControl::SetSpeedPercent(int fan, int percent)
{
    int p = percent;
    if (p > 100)
    {
        p = 100;
    }
    else if (p < 0)
    {
        p = 0;
    }

    if (fan == 0)//Fan 1
    {
        Fan1_Speed_Percent = p;
        ledcWrite(FAN1_PWM_PIN, ConvertPercentTo8Bit(Fan1_Speed_Percent));
    }
    else if (fan == 1)//Fan 2
    {
        Fan2_Speed_Percent = p;
        ledcWrite(FAN2_PWM_PIN, ConvertPercentTo8Bit(Fan2_Speed_Percent));
    }
    else
    {
        //WTF ONLY 2 FANS ATM
        //Reset Fans
        Fan1_Speed_Percent = 0;
        ledcWrite(FAN1_PWM_PIN, ConvertPercentTo8Bit(Fan1_Speed_Percent));
        Fan2_Speed_Percent = 0;
        ledcWrite(FAN2_PWM_PIN, ConvertPercentTo8Bit(Fan2_Speed_Percent));
    }
}

//Get Fan Speed in Percent
int FanControl::GetSpeedPercent(int fan)
{
    if (fan == 0)//Fan 1
    {
        return Fan1_Speed_Percent;
    }
    else if (fan == 1)//Fan 2
    {
        return Fan2_Speed_Percent;
    }
    else
    {
        //WTF ONLY 2 FANS ATM
        return 0;
    }
}



// Interrupt service routine for RPM Fan1
void FanControl::Fan1_RpmISR()
{
    Fan1_RPM_PulseCount = Fan1_RPM_PulseCount + 1;
    //Serial.println("Fan1_RpmISR Exec!");
}

// Interrupt service routine for RPM Fan2
void FanControl::Fan2_RpmISR()
{
    Fan2_RPM_PulseCount = Fan2_RPM_PulseCount + 1;
    //Serial.println("Fan2_RpmISR Exec!");
}


//Get Fan RPM
int FanControl::GetRpm(int fan)
{
    if (fan == 0)//Fan 1
    {
        return Fan1_RPM;
    }
    else if (fan == 1)//Fan 2
    {
        return Fan2_RPM;
    }
    return 0;
}



//Convert Percent(0-100) to 8 Bit resolution(0-255)
int FanControl::ConvertPercentTo8Bit(int percent)
{
    return map(percent, 0, 100, 0, 255);
}