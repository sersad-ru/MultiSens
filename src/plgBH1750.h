/*
Arduino Multi Senstor Tester
(c)2023-2026 by Sergey Sadovnikov (sersad@gmail.com)

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
* BH1750 plugin receives ambient light intensity from the BH1750 sensor.
* Connection:
* VCC (+5V) - red
* GND - black 
* SCL - P7 - gray-black
* SDA - P6 - yellow-black
* 
* Results are printed on screen and sends to serial in human readable and SerialPlotter compartible format.
* 
* You can specify the delay between sensor calls using READ_DELAY_MS in plgBH1750.cpp
* (Keep in mind that the sensor needs about 120 ms to measure light intensity).
* BH1750 I²C address in BH_ADDRESS (plgBH1750.cpp)
* 
*/

/*
struct{
    
} plgBH1750Cfg;
*/
void plgBH1750();
