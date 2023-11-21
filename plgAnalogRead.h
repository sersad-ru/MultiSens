#pragma once
#include <Arduino.h>
#include "mscore.h"

/*
* The AnalogRead plugin reads analog values from P5 and P6.
* Results are printed on screen and sends to serial in human readable and SerialPlotter compartible format.
* Use UP and DOWN buttons to change poll delay. Avaible values are 10, 50, 100, 250, 500, 1000, 1500 and 2000 ms
* Current delay is displayed after P6 value (use LEFT and RIGTH buttuns to scrool the screen)
* Press and hold SELECT button to store current delay in EEPROM.
* Hold DOWN button and pres RESET to clear stored value and return to defaults. (ATTENTION! All stored data will be cleared. FOR ALL plugins!)
*/

struct{
  int8_t scan_mode;
  
} plgAnalogReadCfg;

void plgAnalogRead();
