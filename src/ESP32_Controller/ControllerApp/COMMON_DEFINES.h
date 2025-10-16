//Doc for used ESP32 Board = https://cdn.shopify.com/s/files/1/1509/1638/files/AZ282_A_19-2_DE_B08BTS62L7.pdf?v=1719330230

//ESP32 Force Download Bootmode = Connect GPIO0 & GND with a 10k OHM Resistor

//ERASE FLASH CMD = python -m esptool --chip esp32 erase_flash




/************************************************COMMON DEFINES***************************************************/

//Update Sensors Time  ------  1000 = 1sec
#define UPDATE_SENSORS_DELAY 1000

//Updater Task Start Delay ----- 1000 = 1sec
#define UPDATE_TASK_START_DELAY 8000


//PWM Fans
#define FAN_PWM_FREQ 25000//25 kHz frequency --------------- 25 kHz = 25000 Hz ----------- Tastrate
#define FAN_PWM_RESOLUTION 8//8-bit resolution (0-255)
#define FAN_MAX_RPM 5000//Noctua NF-A4x10 5V PWM
#define FAN_RPM_SAMPLE_TIME 1000//Sample period time in millisecs




/*****************************************************************************************************************/








/*****************************************************GPIO PIN DEFINES********************************************/

/*** DONT CHANGE ANY PINs without RESOLDER ***/


/********************************** TFT Display Pins(only used Chipselect reset on startup)
PURPLE = CS = GPIO15
ORANGE = T_CS = GPIO17
BLUE = SD_CS = GPIO5
****************************************/
#define TFT_DISPLAY_CS_PIN 15// DISPLAY
#define TFT_DISPLAY_TOUCH_CS_PIN 17 // DISPLAY TOUCH
#define SD_CARD_CS_PIN 5 // SD CARD READER --- NOT USED


/********************************** I2C BUS
SCL = GPIO22
SDA = GPIO21
****************************************/
#define I2C_SCL_PIN 22
#define I2C_SDA_PIN 21
#define SHTSENSOR_ADDRESS 0x44


/********************************** ONE WIRE BUS
PIN = GPIO13
****************************************/
#define ONEWIRE_PIN 13



/********************************** CONTROLLER DOOR OPENED SWITCH
PIN = GPIO14 
****************************************/
#define CONTROLLER_DOOR_PIN 14


//////////////////////// FAN PWM
/********************************** FANs
PWM Fan1 = GPIO26
RPM Fan1 = GPIO25

PWM Fan2 = GPIO27
RPM Fan2 = GPIO12
****************************************/
/*
* NOT ALLOWED!!!!!!!!!!!!
* GPIO 6 (SCK/CLK), GPIO 7 (SDO/SD0), GPIO 8 (SDI/SD1), GPIO 9 (SHD/SD2), GPIO 10 (SWP/SD3), GPIO 11 (CSC/CMD)
*/
#define FAN1_RPM_PIN 25
#define FAN1_PWM_PIN 26
#define FAN2_RPM_PIN 12
#define FAN2_PWM_PIN 27

/*****************************************************************************************************************/