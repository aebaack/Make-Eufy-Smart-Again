#include <ESP8266WiFi.h>        // Wifi library for ESP
#include <BlynkSimpleEsp8266.h> // Blynk library
#include <ArduinoOTA.h>         // Library for over the air updates
//#include <IRremote.h>         // IR remote
#include <IRremoteESP8266.h>    // IR remote lib for esp8266
#include <IRsend.h>             // IR sending
#include <IRrecv.h>
#include <math.h>

// Wifi, OTA, blynk credentials
const char* ssid = "XXXXXXXX";                              // SSID
const char* password = "XXXXXXXXXXXXXXX";                   // Password
char auth[] = "XXXXXXXXXXXX";                               // Token for Esp8266
const char* OTAName = "XXXXXXXXXXXX";                       // OTA name
const char* OTAPassword = "XXXXXXXXXXX";                    // OTA Password

// Prepare IR LED to be used
#define IR_LED 4                // ESP8266 GPIO pin to use for blaster (4 is D2 on nodeMCU devBoard)
#define IR_RECV 5               // ESP8266 GPIO pin to use for reciever (5 is D1 on nodeMCU devBoard)
IRrecv irrecv(IR_RECV);
IRsend irsend(IR_LED);          // Set the GPIO to send IR signals


// Pins for blynk
#define VPIN_clean V1           // Switch AUTO clean ON or OFF
#define VPIN_home V2            // Homing signal
#define VPIN_play V3            // Play/ pause
#define VPIN_dpad V4            // D-pad
#define VPIN_room V5            // Room
#define VPIN_max V6             // Max
#define VPIN_wall V7            // Wall
#define VPIN_timerONOFF V8      // Timer ON/ OFF
#define VPIN_timer V9           // Timer handle
#define VPIN_spot V10           // Spot
#define VPIN_front V15
#define VPIN_left V16
#define VPIN_right V17
#define VPIN_back V18

#define VPIN_everything V21     // Tag: everything can be used to group devices and give one command to switch off/ on everything connected to blynk app
#define VPIN_terminal V25       // Terminal monitoring through Blynk app

#define VPIN_connected V30      // Connection

// Define variables
boolean LampState = 0;
boolean SwitchReset = true;     // Flag indicating that the hardware button has been released
int khz = 38;                   // 38kHz carrier frequency
int Timer_status = 0;

// Start timer
BlynkTimer timer;

// Terminal for Blynk
WidgetTerminal terminal(VPIN_terminal);

// Define all the IR codes:

// Auto cleaning
uint16_t ir_auto[299] = {3000, 2966,  504, 486,  504, 1476,  506, 1476,  504, 486,  504, 1476,  506, 486,  504, 486,  504, 486,  504, 486,  506, 486,  504, 486,  506, 486,  504, 1476,  506, 1476,  504, 486,  504, 1478,  504, 486,  504, 486,  504, 486,  504, 1478,  504, 486,  506, 484,  506, 486,  504, 488,  502, 486,  504, 486,  506, 486,  504, 1478,  504, 1478,  504, 1478,  504, 1476,  506, 484,  506, 1476,  506, 1476,  504, 1478,  506, 1476,  506, 1476,  508, 1476,  504, 1478,  506, 1478,  504, 1476,  504, 486,  504, 1476,  504, 486,  504, 486,  506, 486,  504, 1478,  506, 486,  504, 19784,  2998, 2966,  504, 486,  504, 1478,  504, 1478,  504, 486,  506, 1476,  506, 484,  506, 484,  506, 484,  506, 484,  506, 486,  504, 486,  504, 486,  504, 1478,  504, 1476,  506, 484,  506, 1478,  504, 484,  506, 486,  504, 486,  504, 1476,  506, 486,  504, 486,  504, 486,  506, 486,  504, 486,  506, 484,  504, 486,  504, 1478,  504, 1478,  506, 1478,  504, 1476,  504, 486,  506, 1476,  504, 1478,  504, 1478,  504, 1478,  506, 1476,  504, 1478,  506, 1476,  506, 1478,  506, 1478,  504, 484,  506, 1478,  504, 488,  502, 486,  504, 486,  504, 1478,  504, 486,  504, 19786,  3000, 2966,  504, 486,  506, 1478,  504, 1476,  506, 488,  504, 1478,  506, 486,  504, 486,  504, 486,  504, 488,  504, 486,  504, 486,  504, 486,  504, 1478,  504, 1478,  504, 486,  504, 1476,  504, 486,  504, 486,  506, 484,  506, 1478,  504, 486,  506, 484,  506, 486,  506, 486,  504, 488,  504, 486,  504, 486,  506, 1476,  504, 1478,  504, 1478,  506, 1476,  506, 486,  504, 1478,  504, 1480,  502, 1478,  506, 1476,  504, 1478,  504, 1478,  506, 1476,  504, 1478,  504, 1478,  504, 486,  504, 1478,  504, 486,  506, 486,  506, 486,  506, 1476,  506, 488,  504};  // UNKNOWN 4E7C49A6

// Home
//uint16_t ir_home[83] = {3000,3000, 500,500, 500,1500, 500,1500, 500,500, 500,1500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,1500, 500,1500, 500,1500, 500,1500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,1500, 500,1500, 500,1500, 500,500, 500,1500, 500,1500, 500,1500, 500};  // UNKNOWN E8BEB62D

// Play
uint16_t ir_pause[299] = {2906, 3110,  358, 602,  388, 1596,  356, 1600,  410, 604,  388, 1620,  358, 606,  386, 576,  414, 606,  386, 604,  388, 1622,  360, 604,  388, 578,  360, 1620,  412, 1574,  410, 1594,  388, 1620,  386, 578,  390, 602,  388, 578,  410, 1612,  346, 632,  386, 604,  386, 578,  438, 578,  386, 606,  384, 604,  388, 632,  358, 1596,  386, 1596,  386, 1594,  362, 1620,  384, 1598,  386, 1594,  388, 1596,  382, 1594,  388, 1596,  386, 1596,  360, 1620,  388, 1592,  362, 1596,  386, 1624,  384, 1570,  386, 1626,  384, 578,  388, 628,  388, 1596,  386, 578,  412, 1596,  386, 19904,  2880, 3084,  386, 604,  386, 1598,  386, 1594,  386, 602,  388, 1622,  358, 580,  386, 606,  408, 580,  438, 552,  386, 1598,  412, 580,  410, 684,  306, 1622,  306, 1650,  388, 1594,  386, 1646,  336, 602,  388, 606,  360, 630,  384, 1624,  360, 602,  412, 552,  412, 604,  386, 604,  388, 604,  384, 632,  358, 604,  412, 1572,  386, 1596,  388, 1594,  386, 1598,  358, 1622,  360, 1650,  360, 1568,  414, 1596,  386, 1596,  384, 1596,  360, 1620,  388, 1622,  332, 1592,  420, 1594,  384, 1572,  412, 1570,  412, 656,  336, 604,  334, 1624,  412, 578,  410, 1598,  384, 19902,  2880, 3110,  334, 628,  386, 1624,  362, 1598,  382, 602,  388, 1614,  366, 606,  386, 604,  386, 632,  332, 684,  334, 1620,  360, 658,  308, 654,  362, 1568,  414, 1606,  348, 1626,  384, 1622,  360, 604,  362, 680,  334, 608,  384, 1650,  330, 580,  414, 628,  360, 608,  386, 650,  184, 812,  308, 658,  360, 696,  266, 1596,  412, 1568,  388, 1622,  386, 1682,  272, 1620,  362, 1668,  288, 1694,  312, 1622,  388, 1646,  334, 1622,  332, 1678,  306, 1648,  334, 1744,  266, 1594,  388, 1594,  386, 1596,  330, 660,  282, 764,  330, 1596,  386, 686,  278, 1650,  360};  // UNKNOWN 246E003E

// Pause
uint16_t ir_play[299] = {3000, 2964,  504, 486,  506, 1476,  504, 1476,  506, 486,  506, 1474,  506, 486,  504, 486,  506, 484,  506, 486,  504, 1478,  504, 484,  506, 1478,  504, 1476,  506, 1476,  506, 484,  506, 1476,  506, 486,  504, 486,  506, 484,  506, 1476,  506, 484,  506, 486,  504, 486,  506, 486,  506, 486,  504, 486,  504, 1478,  504, 486,  506, 484,  506, 486,  504, 486,  506, 484,  506, 1476,  506, 1476,  504, 1478,  504, 1478,  504, 1476,  504, 1476,  508, 1476,  504, 1478,  506, 1476,  506, 1476,  506, 1476,  506, 1478,  506, 484,  506, 1476,  506, 484,  506, 484,  506, 19782,  2998, 2964,  506, 486,  506, 1476,  506, 1476,  506, 486,  506, 1476,  504, 484,  506, 484,  506, 486,  504, 484,  506, 1476,  506, 486,  504, 1476,  506, 1476,  506, 1476,  506, 484,  504, 1478,  504, 486,  506, 486,  504, 486,  506, 1476,  506, 484,  506, 486,  506, 484,  504, 484,  506, 484,  506, 484,  504, 1478,  506, 486,  506, 484,  506, 484,  506, 486,  506, 486,  504, 1476,  506, 1476,  506, 1476,  506, 1476,  506, 1476,  506, 1474,  508, 1476,  506, 1478,  504, 1478,  504, 1476,  506, 1476,  506, 1478,  504, 486,  504, 1476,  506, 484,  506, 486,  504, 19784,  2996, 2964,  508, 484,  504, 1476,  506, 1478,  506, 484,  506, 1476,  506, 484,  506, 484,  506, 484,  506, 486,  504, 1476,  506, 486,  504, 1476,  506, 1476,  506, 1476,  504, 486,  504, 1476,  506, 486,  504, 486,  504, 486,  506, 1476,  504, 486,  506, 486,  504, 488,  504, 484,  506, 484,  504, 486,  506, 1476,  504, 486,  504, 484,  506, 484,  506, 484,  506, 486,  504, 1474,  506, 1476,  506, 1476,  506, 1476,  506, 1476,  506, 1478,  504, 1478,  504, 1476,  506, 1476,  506, 1478,  504, 1476,  506, 1476,  506, 484,  506, 1476,  504, 486,  506, 484,  506};  // UNKNOWN 8D251CA2

// D-pad directionality
//UP
uint16_t ir_up[199] = {2976, 2986,  510, 506,  458, 1496,  510, 1472,  510, 506,  482, 1474,  508, 508,  484, 506,  462, 528,  484, 506,  486, 506,  484, 1470,  514, 504,  484, 1470,  510, 1472,  484, 530,  484, 506,  488, 502,  486, 504,  488, 502,  488, 1470,  514, 502,  470, 520,  486, 504,  486, 506,  486, 504,  486, 504,  484, 1470,  510, 508,  484, 506,  484, 508,  486, 1470,  512, 1470,  512, 1470,  514, 1468,  514, 1468,  512, 1470,  514, 1468,  512, 1470,  512, 1470,  514, 1466,  514, 1470,  512, 1468,  516, 502,  488, 504,  486, 504,  488, 1466,  514, 1468,  514, 504,  488, 19820,  2974, 2988,  482, 536,  454, 1500,  484, 1500,  482, 534,  454, 1500,  484, 534,  458, 534,  482, 508,  454, 536,  456, 534,  480, 1474,  482, 534,  480, 1480,  480, 1500,  480, 538,  452, 536,  456, 536,  454, 536,  454, 536,  456, 1502,  482, 534,  456, 536,  456, 534,  456, 534,  454, 536,  480, 512,  456, 1500,  506, 510,  456, 536,  456, 534,  456, 1500,  506, 1476,  480, 1502,  482, 1500,  482, 1500,  480, 1500,  482, 1500,  482, 1500,  480, 1502,  480, 1502,  482, 1500,  506, 1476,  480, 534,  480, 512,  454, 536,  456, 1500,  482, 1498,  482, 536,  456};  // UNKNOWN D7848A6B
//Down
uint16_t ir_down[199] = {2998, 2964,  506, 486,  504, 1478,  504, 1478,  506, 486,  504, 1478,  504, 484,  506, 488,  504, 488,  504, 484,  504, 1478,  504, 1476,  506, 1476,  506, 1476,  504, 1478,  504, 486,  504, 486,  504, 486,  506, 484,  504, 488,  504, 1478,  504, 486,  504, 486,  506, 484,  506, 486,  504, 486,  504, 488,  502, 1478,  504, 486,  506, 486,  504, 486,  504, 1478,  504, 1476,  506, 1476,  504, 1478,  504, 1478,  504, 1476,  506, 1476,  506, 1476,  506, 1476,  506, 1476,  506, 484,  504, 488,  502, 486,  504, 1476,  506, 484,  504, 1478,  502, 1478,  506, 486,  504, 19830,  2998, 2966,  504, 488,  504, 1478,  506, 1476,  506, 486,  504, 1478,  504, 484,  506, 486,  504, 486,  506, 486,  504, 1478,  506, 1474,  506, 1478,  504, 1476,  506, 1476,  506, 486,  504, 486,  506, 486,  504, 486,  504, 486,  504, 1478,  504, 486,  504, 488,  504, 486,  506, 486,  504, 486,  504, 486,  504, 1476,  506, 488,  504, 486,  504, 486,  504, 1478,  506, 1478,  506, 1476,  506, 1476,  506, 1476,  504, 1478,  504, 1476,  504, 1478,  506, 1476,  506, 1476,  506, 486,  506, 486,  502, 488,  504, 1476,  506, 486,  506, 1476,  506, 1478,  504, 486,  504};  // UNKNOWN E083952B
//Right
uint16_t ir_right[199] = {2998, 2966,  504, 488,  502, 1478,  504, 1478,  504, 488,  504, 1478,  504, 488,  478, 512,  504, 486,  504, 486,  506, 1474,  504, 1478,  504, 486,  504, 1478,  506, 1476,  504, 488,  502, 486,  504, 488,  480, 512,  502, 488,  502, 1478,  504, 486,  502, 490,  504, 486,  502, 486,  506, 484,  504, 486,  504, 1478,  504, 488,  504, 486,  504, 1476,  506, 488,  504, 486,  504, 1478,  504, 1480,  504, 1476,  504, 1478,  506, 1476,  504, 1478,  506, 1474,  506, 1478,  504, 488,  502, 488,  504, 486,  504, 486,  504, 488,  502, 1478,  506, 1476,  506, 1478,  504, 19862,  3000, 2964,  504, 488,  504, 1478,  504, 1478,  506, 486,  480, 1502,  504, 488,  502, 488,  502, 488,  480, 510,  502, 1478,  482, 1500,  504, 488,  502, 1476,  506, 1478,  504, 488,  502, 488,  506, 486,  502, 488,  502, 488,  502, 1478,  504, 488,  502, 488,  480, 512,  504, 488,  504, 486,  504, 486,  504, 1476,  506, 488,  502, 488,  504, 1478,  480, 510,  504, 486,  504, 1476,  504, 1478,  504, 1476,  504, 1478,  504, 1476,  506, 1476,  506, 1476,  506, 1474,  506, 488,  502, 486,  504, 488,  504, 486,  504, 484,  506, 1478,  504, 1476,  504, 1478,  504};  // UNKNOWN 1291E90F
//Left
uint16_t ir_left[199] = {2998, 2964,  506, 486,  504, 1478,  504, 1476,  506, 484,  506, 1476,  506, 486,  504, 486,  504, 486,  504, 486,  504, 488,  504, 1474,  506, 1478,  504, 1478,  504, 1476,  506, 486,  504, 486,  504, 486,  504, 486,  504, 486,  504, 1478,  506, 486,  506, 486,  504, 484,  506, 486,  504, 486,  506, 486,  506, 1478,  504, 484,  506, 486,  504, 1478,  506, 486,  504, 486,  504, 1476,  506, 1478,  504, 1478,  504, 1476,  506, 1476,  506, 1476,  504, 1478,  506, 1476,  506, 1476,  504, 1478,  504, 486,  506, 1476,  506, 484,  506, 1476,  504, 1476,  504, 1478,  504, 19828,  2998, 2964,  504, 486,  506, 1476,  504, 1476,  504, 484,  506, 1478,  504, 486,  504, 486,  504, 486,  504, 486,  506, 486,  504, 1478,  504, 1478,  506, 1476,  506, 1478,  506, 484,  506, 486,  504, 484,  506, 486,  506, 484,  504, 1476,  506, 486,  504, 486,  504, 486,  504, 486,  504, 484,  506, 486,  504, 1476,  506, 486,  504, 484,  506, 1478,  504, 484,  506, 486,  504, 1478,  504, 1478,  504, 1478,  506, 1476,  504, 1478,  506, 1478,  504, 1476,  506, 1476,  506, 1476,  506, 1476,  506, 486,  504, 1478,  504, 486,  504, 1478,  504, 1478,  504, 1478,  504};  // UNKNOWN 29E12407

// Single room
uint16_t ir_room[299] = {3000, 2964,  506, 484,  506, 1476,  506, 1476,  506, 484,  506, 1476,  506, 484,  506, 486,  504, 484,  506, 1476,  506, 484,  506, 1476,  506, 484,  506, 1478,  506, 1476,  506, 486,  504, 1476,  506, 484,  506, 484,  506, 484,  506, 1476,  506, 486,  504, 486,  506, 486,  502, 488,  504, 486,  504, 486,  506, 1478,  504, 484,  506, 486,  506, 486,  502, 486,  506, 1476,  506, 1476,  506, 1476,  506, 1476,  506, 1476,  504, 1476,  506, 1476,  506, 1476,  506, 1476,  506, 486,  504, 1476,  506, 484,  506, 484,  506, 484,  506, 1476,  504, 486,  506, 1476,  506, 19782,  3000, 2966,  504, 486,  506, 1476,  504, 1476,  506, 484,  506, 1476,  506, 484,  506, 484,  506, 486,  506, 1478,  504, 486,  506, 1476,  506, 486,  504, 1476,  506, 1474,  506, 484,  506, 1476,  506, 484,  506, 484,  506, 486,  504, 1476,  506, 484,  506, 484,  506, 486,  504, 486,  506, 486,  506, 484,  506, 1476,  506, 486,  504, 486,  506, 484,  506, 486,  506, 1478,  504, 1476,  506, 1476,  504, 1478,  504, 1478,  504, 1478,  504, 1476,  506, 1476,  508, 1474,  506, 486,  504, 1478,  504, 484,  506, 484,  508, 484,  506, 1478,  504, 486,  504, 1478,  504, 19784,  3000, 2966,  504, 488,  504, 1476,  506, 1476,  506, 486,  506, 1476,  506, 486,  504, 486,  506, 486,  504, 1476,  506, 484,  506, 1476,  504, 486,  506, 1476,  506, 1476,  506, 484,  506, 1478,  506, 482,  506, 484,  506, 486,  504, 1478,  504, 486,  504, 486,  506, 484,  506, 486,  506, 484,  506, 486,  506, 1476,  506, 486,  504, 486,  506, 486,  504, 486,  504, 1478,  504, 1478,  504, 1476,  506, 1476,  506, 1478,  504, 1476,  504, 1476,  504, 1476,  510, 1472,  506, 484,  504, 1476,  506, 486,  506, 484,  506, 484,  506, 1476,  504, 484,  506, 1478,  506};  // UNKNOWN C70DC523

// Max cleaning
//uint16_t ir_max[83] = {3000,3000, 500,500, 500,1500, 500,1500, 500,500, 500,1500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,1500, 500,500, 500,500, 500,1500, 500,1500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,500, 500,1500, 500,1500, 500,1500, 500,1500, 500,500, 500,1500, 500,1500, 500};  // UNKNOWN A2B091CB

// Wall cleaning
uint16_t ir_wall[299] = {2998, 2966,  504, 486,  506, 1476,  506, 1476,  506, 488,  504, 1476,  504, 486,  504, 486,  504, 488,  502, 1478,  504, 486,  506, 486,  504, 1476,  506, 1476,  506, 1476,  506, 486,  504, 486,  504, 486,  504, 486,  504, 486,  506, 1476,  506, 486,  504, 486,  506, 484,  506, 484,  506, 486,  504, 486,  504, 1478,  504, 486,  504, 486,  504, 486,  506, 1476,  504, 486,  504, 1476,  506, 1476,  506, 1476,  508, 1476,  504, 1480,  502, 1478,  504, 1476,  506, 1478,  504, 486,  506, 486,  504, 1476,  506, 1476,  506, 486,  504, 1478,  506, 486,  506, 1476,  506, 19786,  2998, 2964,  506, 486,  504, 1476,  506, 1478,  506, 486,  504, 1478,  504, 486,  506, 486,  504, 488,  504, 1476,  506, 484,  506, 484,  506, 1476,  506, 1476,  506, 1478,  504, 486,  506, 484,  506, 484,  506, 484,  506, 484,  506, 1476,  506, 484,  506, 486,  504, 488,  506, 484,  506, 484,  506, 484,  506, 1478,  504, 486,  506, 486,  504, 486,  502, 1478,  506, 488,  504, 1476,  506, 1478,  504, 1478,  504, 1478,  504, 1478,  504, 1478,  506, 1476,  504, 1478,  506, 486,  504, 486,  504, 1478,  504, 1478,  506, 484,  506, 1476,  506, 484,  506, 1478,  504, 19788,  2998, 2966,  506, 486,  504, 1476,  506, 1476,  506, 486,  506, 1476,  506, 484,  506, 486,  504, 486,  506, 1476,  506, 486,  504, 484,  506, 1476,  504, 1478,  506, 1476,  506, 486,  504, 486,  506, 486,  504, 486,  506, 486,  504, 1478,  506, 484,  504, 486,  504, 488,  504, 484,  506, 484,  506, 484,  506, 1476,  504, 486,  506, 484,  506, 486,  504, 1478,  504, 486,  506, 1476,  506, 1478,  504, 1476,  506, 1476,  504, 1476,  506, 1478,  504, 1478,  506, 1476,  506, 486,  504, 486,  504, 1476,  506, 1478,  504, 486,  506, 1478,  504, 484,  506, 1478,  506};  // UNKNOWN C949C241

// Spot cleaning
uint16_t ir_spot[299] = {3000, 2964,  506, 486,  506, 1478,  504, 1476,  506, 486,  504, 1478,  506, 484,  506, 486,  506, 486,  504, 1476,  504, 486,  504, 484,  506, 486,  504, 1476,  506, 1476,  506, 486,  504, 486,  506, 486,  504, 486,  504, 486,  506, 1476,  504, 486,  504, 486,  506, 486,  504, 486,  504, 486,  506, 484,  504, 1478,  506, 484,  506, 486,  506, 486,  504, 1478,  504, 486,  506, 1476,  506, 1478,  504, 1478,  506, 1476,  506, 1476,  506, 1476,  506, 1478,  504, 1478,  506, 484,  506, 484,  506, 1476,  504, 488,  504, 484,  506, 1476,  506, 484,  506, 1478,  504, 19782,  3000, 2966,  504, 484,  506, 1476,  504, 1478,  504, 486,  504, 1478,  504, 486,  504, 486,  504, 486,  506, 1478,  504, 484,  506, 486,  506, 484,  506, 1476,  506, 1476,  506, 484,  506, 484,  506, 486,  504, 486,  506, 486,  506, 1476,  506, 484,  504, 486,  504, 486,  506, 486,  504, 486,  506, 484,  506, 1476,  506, 486,  506, 484,  504, 486,  504, 1476,  506, 484,  506, 1476,  506, 1478,  506, 1476,  504, 1476,  506, 1478,  504, 1476,  506, 1476,  506, 1478,  504, 486,  506, 484,  506, 1476,  506, 484,  506, 486,  506, 1478,  504, 486,  504, 1476,  506, 19782,  3000, 2964,  506, 486,  504, 1474,  506, 1478,  504, 484,  506, 1474,  506, 484,  506, 484,  506, 484,  506, 1476,  506, 484,  506, 484,  506, 484,  506, 1478,  504, 1476,  506, 484,  504, 486,  506, 484,  506, 484,  506, 486,  506, 1478,  504, 486,  506, 486,  504, 486,  506, 486,  504, 486,  506, 484,  506, 1476,  506, 486,  504, 484,  506, 486,  504, 1476,  506, 486,  504, 1478,  504, 1478,  504, 1476,  504, 1478,  504, 1478,  504, 1478,  504, 1478,  504, 1478,  504, 486,  506, 484,  504, 1476,  506, 486,  504, 484,  508, 1474,  506, 484,  506, 1476,  506};  // UNKNOWN 12AE912D

void setup()
{
  // Serial port
  //Serial.begin(74880, SERIAL_8N1, SERIAL_TX_ONLY);
  Serial.begin(74880);
  Serial.println("Starting vacuum cleaner controller");

  // Wifi connection and services
  startWiFi();                      // Connect to WIFI
  startOTA();                       // Start OTA servers

  // Check internet connection every 5 minutes
  timer.setInterval(300000L, CheckConnection); // check if still connected every 5 minutes  

  // Start configuration
  Blynk.config(auth);               // Connect to Blynk
  Blynk.connect();

  // WDT handling
  ESP.wdtDisable();
  ESP.wdtEnable(WDTO_8S);
  timer.setInterval(100000, WDT);

  irsend.begin();
  irrecv.enableIRIn(); // Start the receiver
}

void CheckConnection(){    // check every 5 minutes if connected to Blynk server
  if(!Blynk.connected()){
    Serial.println("Not connected to Blynk server"); 
    Blynk.connect();  // try to connect to server with default timeout
  }
  else{
    Serial.println("Connected to Blynk server");  
    terminal.println("Connected to Blynk server");  
    Blynk.virtualWrite(VPIN_connected, 1);   // Denote that ESP is connected to Blynk server
  }
}

void loop() {
  if(Blynk.connected()){
    Blynk.run();
  }
  ArduinoOTA.handle();
  timer.run();
}

BLYNK_CONNECTED() {
    Blynk.syncAll();
}

// Start cleaning in auto mode
BLYNK_WRITE(VPIN_clean){
  int Clean_button = param.asInt();
  if (Clean_button == 1){
    irsend.sendRaw(ir_auto, 299, khz);
    Blynk.virtualWrite(VPIN_play, 1);   // Denote that vacuum cleaner is in a cleaning mode
    delay(300);
    terminal.println("Eufy will clean the rooms");
  }
}

/*
// Send Vacuum cleaner home
BLYNK_WRITE(VPIN_home){
  int Clean_button = param.asInt();
  if (Clean_button == 1){
    irsend.sendRaw(ir_home, 83, khz);   // Send Home signal on press
    Blynk.virtualWrite(VPIN_play, 0);   // Denote that vacuum cleaner is not in a cleaning mode
    delay(300);
    terminal.println("Bedeep boop beep, going home");
  }
}
*/

//
BLYNK_WRITE(VPIN_play){
  int Clean_button = param.asInt();
  if (Clean_button == 1){
    irsend.sendRaw(ir_play, 299, khz);   // Send Play signal on press
    Blynk.virtualWrite(VPIN_play, 1);   // Denote that vacuum cleaner is in a cleaning mode
    delay(300);
    terminal.println("Resuming the robot");
    }
    else if (Clean_button == 0) {
    irsend.sendRaw(ir_pause, 299, khz);   // Send Pause signal on press
    Blynk.virtualWrite(VPIN_play, 0);   // Denote that vacuum cleaner is not in a cleaning mode
    delay(300);
    terminal.println("Pausing the robot");
    }
}

BLYNK_WRITE(VPIN_front){
  int Clean_button = param.asInt();
  if (Clean_button == 1){
    irsend.sendRaw(ir_up, 199, khz);
    delay(300);
  }
}

BLYNK_WRITE(VPIN_left){
  int Clean_button = param.asInt();
  if (Clean_button == 1){
    irsend.sendRaw(ir_left, 199, khz);
    delay(300);
  }
}

BLYNK_WRITE(VPIN_right){
  int Clean_button = param.asInt();
  if (Clean_button == 1){
    irsend.sendRaw(ir_right, 199, khz);
    delay(300);
  }
}

/*
BLYNK_WRITE(VPIN_down){
  int Clean_button = param.asInt();
  if (Clean_button == 1){
    irsend.sendRaw(ir_down, 199, khz);
    delay(300);
  }
}
*/

/*
// Navigate using the D-PAD
BLYNK_WRITE(VPIN_dpad){
  float x_pos = param[0].asFloat();
  float y_pos = param[1].asFloat();
  int x = round(x_pos); //Rounding to get only directional answers
  int y = round(y_pos);

  if (x == 1 && y == 0){ // right
    irsend.sendRaw(ir_right, 199, khz);   // Send Right signal on press
    Blynk.virtualWrite(VPIN_play, 1);   // Denote that vacuum cleaner is in a cleaning mode
    terminal.println("Wooo...going right");
  }
  else if (x == -1 && y == 0){ //left
    irsend.sendRaw(ir_left, 199, khz);   // Send Left signal on press
    Blynk.virtualWrite(VPIN_play, 1);   // Denote that vacuum cleaner is in a cleaning mode
    terminal.println("Wooo...going left");
  }
  else if (x == 0 && y == 1){ //up
    irsend.sendRaw(ir_up, 199, khz);   // Send Up signal on press
    Blynk.virtualWrite(VPIN_play, 1);   // Denote that vacuum cleaner is in a cleaning mode
    terminal.println("Wooo...going up");
  }
  else if (x == 0 && y == -1){ //down
    irsend.sendRaw(ir_down, 199, khz);   // Send Down signal on press
    Blynk.virtualWrite(VPIN_play, 1);   // Denote that vacuum cleaner is in a cleaning mode
    terminal.println("Wooo...going down");
  }
  else if (x == -1 && y == -1){ //left, down
    irsend.sendRaw(ir_down, 199, khz);   // Send Down signal on press
    irsend.sendRaw(ir_left, 199, khz);   // Send Left signal on press
    Blynk.virtualWrite(VPIN_play, 1);   // Denote that vacuum cleaner is in a cleaning mode
    terminal.println("Wooo...going left-down");
  }
}
*/

// Start cleaning in room mode
BLYNK_WRITE(VPIN_room){
  int Clean_button = param.asInt();
  if (Clean_button == 1){
    irsend.sendRaw(ir_room, 299, khz);   // Send Room Cleaning signal on press
    Blynk.virtualWrite(VPIN_play, 1);   // Denote that vacuum cleaner is in a cleaning mode
    terminal.println("Clean you room!");
  }
}

/*
// Start cleaning in max mode
BLYNK_WRITE(VPIN_max){
  int Clean_button = param.asInt();
  if (Clean_button == 1){
    irsend.sendRaw(ir_max, 83, khz);   // Send Max Cleaning signal on press
    Blynk.virtualWrite(VPIN_play, 1);   // Denote that vacuum cleaner is in a cleaning mode
    terminal.println("Cleaning for the queen!");
  }
}
*/

// Start cleaning in wall mode
BLYNK_WRITE(VPIN_wall){
  int Clean_button = param.asInt();
  if (Clean_button == 1){
    irsend.sendRaw(ir_wall, 299, khz);   // Send Wall Cleaning signal on press
    Blynk.virtualWrite(VPIN_play, 1);   // Denote that vacuum cleaner is in a cleaning mode
    terminal.println("Wall clean!");
  }
}

// Start cleaning in spot mode
BLYNK_WRITE(VPIN_spot){
  int Clean_button = param.asInt();
  if (Clean_button == 1){
    irsend.sendRaw(ir_spot, 299, khz);   // Send Spot Cleaning signal on press
    Blynk.virtualWrite(VPIN_play, 1);   // Denote that vacuum cleaner is in a cleaning mode
    terminal.println("Spot clean!");
  }
}

/*
// Start cleaning according to a timer
BLYNK_WRITE(VPIN_timerONOFF){
  int Timer_status = param.asInt();
}

// Start cleaning according to a timer, only if the timer status is set to be used
BLYNK_WRITE(VPIN_timer){
  int Clean_button = param.asInt();
  if (Timer_status == 1 && Clean_button == 1){
    irsend.sendRaw(ir_auto, 83, khz);   // Send Auto Clean signal on press
    Blynk.virtualWrite(VPIN_play, 1);   // Denote that vacuum cleaner is in a cleaning mode
    terminal.println("I will clean it later!");
  }
}
*/

// Feed the watchdog!
void WDT(){
  ESP.wdtFeed();
}

// Connect wifi
void startWiFi() { // Start a Wi-Fi access point, and try to connect to some given access points. Then wait for either an AP or STA connection
  WiFi.begin(ssid, password);                 // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {     // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
    i = i++;
    if (i > 300){
      break; // Exit after 5 minutes
    }
  }
  Serial.println('\n');
  Serial.println("Connection established!");
  Serial.print("IP address:\t");
  terminal.println("Connection established, IP address:\t");
  terminal.print(WiFi.localIP());
  Serial.println(WiFi.localIP());             // Print IP address
}

// Start to OTA server
void startOTA() {                             // Start the OTA service
  ArduinoOTA.setHostname(OTAName);            // Hostname for OTA
  ArduinoOTA.setPassword(OTAPassword);        // Password for OTA

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
    terminal.println("Starting firmware flash");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\r\nOTA Flash complete");
    terminal.println("\r\nOTA Flash is complete");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    terminal.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA server ready\r\n");
}
