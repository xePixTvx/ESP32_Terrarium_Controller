//Doc for used ESP32 Board = https://cdn.shopify.com/s/files/1/1509/1638/files/AZ282_A_19-2_DE_B08BTS62L7.pdf?v=1719330230

//ESP32 Force Download Bootmode = Connect GPIO0 & GND with a 10k OHM Resistor





/*****************************************************TFT DISPLAY PIN DEFINES********************************************/

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
BLUE = SD_CS = GPIO5                ---- DONE ---- NOT USED
****************************************/
#define TFT_DISPLAY_CS_PIN 15// DISPLAY
#define TFT_DISPLAY_TOUCH_CS_PIN 17 // DISPLAY TOUCH
#define TFT_DISPLAY_SCK 18
#define TFT_DISPLAY_MISO 19
#define TFT_DISPLAY_MOSI 23

#define SD_CARD_CS_PIN 5 // SD CARD READER --- NOT USED

/*****************************************************************************************************************/