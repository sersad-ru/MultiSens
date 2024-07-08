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
* The TSOP 312 plugin receives signals encoded with NEC protocol from an Infrared Remote Control Unit 
* using the TSOP312x receiver.
* 
* Results are decoded and displayed on the device screen and sends to the serial in human readable format. 
*
* 
* 
* Connection:
* GND - GND - black
* Vs  - +5V - red
* OUT - P0 - green
*
*/
/*
struct{
    
} plgTSOP312Cfg;
*/
void plgTSOP312();
