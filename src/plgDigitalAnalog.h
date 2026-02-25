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
* DigitalAnalog plugin reads digital pin P0 and analog pin P5.
* P0 is configured as INPUT
* Results are printed on screen and sends to serial in human readable and SerialPlotter compartible format.
* Use UP and DOWN buttons to change poll delay. Avaible values are 10, 50, 100, 250, 500, 1000, 1500 and 2000 ms
* Additional mode is INTERRUPT mode. Interrupts are configured as CHANGE on both pins. In interrupt mode values are printed only if an interrupt occurs.
* Not only values are printed to serial but time (in ms) since last changes of values.
* Current delay is displayed after P5 value (use LEFT and RIGTH buttuns to scrool the screen)
* Press and hold SELECT button to store current delay in EEPROM.
* Hold DOWN button and pres RESET to clear stored value and return to defaults. (ATTENTION! All stored data will be cleared. FOR ALL plugins!)
*/

struct{
  uint8_t scan_mode;
  
} plgDigitalAnalogCfg;

void plgDigitalAnalog();
