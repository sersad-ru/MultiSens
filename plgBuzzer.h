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
* The Buzzer plugin generates a tone with the **passive** buzzer. 
* Only a passive buzzer can work at various frequency. The range of frequencies are 
* from `31` Hz to `8` MHz.
*
* Current frequency displayed on the device screen and sends to the serial in human-readable format.
*
* Press the `SELECT` button to start and stop tone generation.
* Use the `UP` and `DOWN` buttons to change the frequency. 
* Press and hold the the `SELECT` button to store current frequency in the the `EEPROM`.
*
* Hold the `DOWN` button and press `RESET` to clear stored value and return to defaults. 
*  (**ATTENTION!** All stored data will be cleared. **FOR ALL** plugins!)
* 
* 
* Connection:
* GND - GND - black
* VCC - +5V - red
* IN  - P1  - blue
* 
*/


struct{
  uint16_t freq; // frequiency
} plgBuzzerCfg;

void plgBuzzer();
