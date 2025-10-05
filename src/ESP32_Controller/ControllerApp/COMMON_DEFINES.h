//Doc for used ESP32 Board = https://cdn.shopify.com/s/files/1/1509/1638/files/AZ282_A_19-2_DE_B08BTS62L7.pdf?v=1719330230

//ESP32 Force Download Bootmode = Connect GPIO0 & GND with a 10k OHM Resistor




/************************************************COMMON DEFINES***************************************************/

//0 = USE UI & Touch ----- 1 = Disable UI & Touch(for testing sensors etc......)
#define DISABLE_UI_AND_TOUCH 0

//0 = USE EEZ UI ----- 1 = USE LVGL TEST UI
#define LVGL_UI_TESTING 0

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

/********************************** TFT DISPLAY
//Common
RED = 3.3V                          ---- DONE
BLACK = GND                         ---- DONE
PURPLE = CS = GPIO15                ---- DONE
BLUE = RESET = GPIO4                ---- DONE
GREEN = DC = GPIO2                  ---- DONE
WHITE = SDI/MOSI/COPI = GPIO23      ---- DONE
YELLOW = SCK = GPIO18               ---- DONE
GREY = SDO/MISO/CIPO = GPIO19       ---- DONE

//Touch
ORANGE = T_CS = GPIO17              ---- DONE
BROWN = T_IRQ = NOT NEEDED

//SD Card
BLUE = SD_CS = GPIO5                ---- DONE
****************************************/
#define TFT_DISPLAY_CS_PIN 15// DISPLAY
#define TFT_DISPLAY_TOUCH_CS_PIN 17 // DISPLAY TOUCH
#define TFT_DISPLAY_SCK 18
#define TFT_DISPLAY_MISO 19
#define TFT_DISPLAY_MOSI 23

#define SD_CARD_CS_PIN 5 // SD CARD READER


/********************************** I2C BUS
SCL = GPIO22
SDA = GPIO21
****************************************/


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