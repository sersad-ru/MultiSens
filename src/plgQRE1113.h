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
* QRE1113 plugin recieves distance information from the QRE1113 sensor.
* Connection:
* GND - GND - black
* VCC - +5V - red
* OUT - P0 - green  
*
* 
* Results are printed on the screen and sends to serial in human readable format.* 
* Use UP and DOWN buttons to change poll delay. Avaible values are 10, 50, 100, 250, 500, 1000, 1500 and 2000 ms
* Press and hold SELECT button to store current delay in EEPROM.
* Hold DOWN button and pres RESET to clear stored value and return to defaults. (ATTENTION! All stored data will be cleared. FOR ALL plugins!)
*  
*/


struct{
  int8_t scan_mode;    
} plgQRE1113Cfg;

void plgQRE1113();
