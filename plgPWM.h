#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* The PWM plugin create PWM on P1, P2 and P3 (software PWM) pins
* Use SELECT button to change the channel.
* Use UP and DOWN button to change duty on selected channel.
* Press and hold SELECT button to store current duty on each channel in EEPROM.
* Hold DOWN button and pres RESET to clear stored value and return to defaults. (ATTENTION! All stored data will be cleared. FOR ALL plugins!)
*/

struct{
  uint8_t duty[3];
  
} plgPWMCfg;

void plgPWM();
