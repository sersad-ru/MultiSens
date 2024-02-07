#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* The Stepper plugin drives 28BYj-48 stepper motor with ULN2003 driver.
* 
* Connection:
* GND - GND - black
* VCC - +5V - red
* IN1 - P0 - green
* IN2 - P1 - blue
* IN3 - P2 - yellow
* IN3 - P3 - violet
*
* Use UP and DOWN button to change the angle.
* Use SELECT button to start rotation.
* Attention! To prevent overload of the device, connect only one servo drive without any load.
* Press and hold SELECT button to store current angle in EEPROM.
* Hold DOWN button and pres RESET to clear stored value and return to defaults. (ATTENTION! All stored data will be cleared. FOR ALL plugins!)
*/
/*
struct{
  uint8_t angle;
  
} plgStepperCfg;
*/
void plgStepper();
