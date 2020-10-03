// This file contains the configuration for the "Bahnsteiganzeige"

#define FIRST_DCC_ADDR          5       // First used DCC accessory message
#define LAST_DCC_ADDR           21      // Last    "         "


#define DCC_SIGNAL_PIN          2       // Connected to the opto coppler which reads the DCC signals

#define BUTTON0_PIN             3       // Activate the next text message from Flash fo the current OLED
#define BUTTON1_PIN             4       // Switch to the next OLED
#define BUTTON2_PIN             5       // Next text message for OLED 0
#define BUTTON3_PIN             6       //   "           "            1
#define BUTTON4_PIN             7       //   "           "            2

#define RESET_DISP_PIN          A1      // Reset PIN for the OLED Displays if used with Hardis OLED Adapter
#define UNUSED_AIN_PIN          A3      // This analog input is used to generate a random initial value for the random() function

const PROGMEM            //Pin Rail Side
Rail_Cfg_T Rail_Cfg[] = {//Nr  Nr
                          { 8, "3", 'L' },
//                          { 9, "4", 'R' },   // Uncomment this lines to use 3 OLED pannels
//                          {10, "5", 'L' },   // and add additional lines if more then 3 pannels are used
                        };

// If the following two lines are enabled the displayed text will change randomly
//#define RAND_CHANGE_MINTIME   20      // Minimal time between two display changes [s]
//#define RAND_CHANGE_MAXTIME   60      // Maximal time between two display changes [s]

#define LED_HEARTBEAT_PIN       13      // Define the pin for the heartbeat LED.

#define SERIAL_BAUDRATE         115200  // Attention the serial monitor of the Arduino IDE must use the same baud rate

#define USE_DCC                 1       // FLASH Usage: 4014 (13%), RAM 70 Byte  02.02.19:

#define USE_U8G2LIB             1       // Use the new u8g2 library (If changed restart the IDE or open the library manager to rebuild all)

#define  _PRINT_DEBUG_MESSAGES  0       // Set to 1 to print debug messages

