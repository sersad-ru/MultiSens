#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* The Servo plugin rotates servo drive to the selected angle (from 0 to 180°).
* 
* Connection:
* GND - GND - black + 470uF to +5V
* VCC - +5V - red
* PWM - P0 - green
*
* Use UP and DOWN button to change the angle.
* Use SELECT button to start rotation.
* Attention! To prevent overload of the device, connect only one servo drive without any load.
* Press and hold SELECT button to store current angle in EEPROM.
* Hold DOWN button and pres RESET to clear stored value and return to defaults. (ATTENTION! All stored data will be cleared. FOR ALL plugins!)
*/

struct{
  uint8_t angle;
  
} plgServoCfg;

void plgServo();
