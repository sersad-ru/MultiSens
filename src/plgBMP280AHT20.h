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
* BMP280/AHT20 plugin receives temperature, humidity and pressure from the BMP280+AHT20 module.
* Connection:
* GND - black* 
* VCC (+5V) - red
* SDA - P6 - yellow-black
* SCL - P7 - gray-black
* 
* Results are printed on screen and sends to serial in human readable and SerialPlotter compartible format.
* BMP280 used for pressure only. AHT20 for temperature and humidity.
* 
* You can specify the delay between sensor calls using READ_DELAY_MS in plgBMP280AHT20.cpp
* BMP280 I²C address in BMP_ADDRESS (plgBMP280AHT20.cpp)
* AHT20 I²C address in AHT_ADDRESS (plgBMP280AHT20.cpp)
* 
*/

/*
struct{
    
} plgBMP280AHT20Cfg;
*/
void plgBMP280AHT20();
