/*
Arduino Multi Sensor Tester
(c)2023-2025 by Sergey Sadovnikov (sersad@gmail.com)

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
* The MAX9814 plugin collects data from the MAX9814 microphone sensor. 
* It attempts to detect the harmonic frequency in the signal from 650 to 9500 Hz. 
* If successful, it displays the frequency and volume level on the screen.
*
* This plugin is based on Arduino-FrequencyDetector library by Armin Joachimsmeyer
* https://github.com/ArminJo/Arduino-FrequencyDetector
*
*
* Connection:
* GND - black 
* Vdd (+5V) - red
* Out - P5  - orange
* 
* Results are printed on screen and sends to serial in human readable and SerialPlotter compartible format.
* 
* You can specify the delay between display outputs using `DISPLAY_DELAY_MS` in plgMAX9814.cpp

*/

/*
struct{
    
} plgMAX9814Cfg;
*/
void plgMAX9814();
