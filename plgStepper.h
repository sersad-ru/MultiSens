/*
Arduino Multi Senstor Tester
(c)2023-2024 by Sergey Sadovnikov (sersad@gmail.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <www.gnu.org/licenses/>.
*/

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

* Press and hold SELECT button to store current angle in EEPROM.
* Hold DOWN button and pres RESET to clear stored value and return to defaults. (ATTENTION! All stored data will be cleared. FOR ALL plugins!)
*/

struct{
  int16_t angle;  
} plgStepperCfg;

void plgStepper();
