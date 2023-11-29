#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* QRE1113 plugin recieve distance information QRE1113 from sensor.
* Connection:
* GND - GND - black
* VCC - +5V - red
* OUT - P0 - green  
*
* 
* Results are printed on the screen and sends to serial in human readable format.* 
* Use UP and DOWN buttons to change poll delay. Avaible values are 10, 50, 100, 250, 500, 1000, 1500 and 2000 ms
* Current delay is displayed after P6 value (use LEFT and RIGTH buttuns to scrool the screen)
* Press and hold SELECT button to store current delay in EEPROM.
* Hold DOWN button and pres RESET to clear stored value and return to defaults. (ATTENTION! All stored data will be cleared. FOR ALL plugins!)
*  
*/


struct{
  int8_t scan_mode;    
} plgQRE1113Cfg;

void plgQRE1113();
