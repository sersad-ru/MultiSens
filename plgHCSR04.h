#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* HC-SR04 plugin recieve distance information from HC-SR04 sensor.
* Connection:
* GND  - GND - black
* VCC  - +5V - red
* Trig - P0 - green  
* Echo - P1 - blue
*
* 
* Results are printed on the screen and sends to serial in human readable format.* 
*  
* You can specify the delay between sensor calls using READ_DELAY_MS in plgHSR04.cpp
* 
* Use UP and DOWN buttons to change current temperature. Avaible rangea is -50 - +50C. 
* Press and hold SELECT button to store current delay in EEPROM.
* Hold DOWN button and pres RESET to clear stored value and return to defaults. (ATTENTION! All stored data will be cleared. FOR ALL plugins!)
*
*/


struct{
  int8_t temp;    
} plgHCSR04Cfg;

void plgHCSR04();
