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
* The TM1637 plugin sends data to the TM1637 LED controller.
*
* The current position and the symbol code are displayed on the device screen and 
* sends to the serial.
*
* Press the `SELECT` key switch between `auto` and `manual` mode.
*
* In `auto` mode symbol codes and symbol position are changed automatically.
*
* In `manual` mode you can press the `UP` and the `DOWN` keys to step back and forward 
* in code change sequence.
*
* You can specify the delay between steps in code change sequence using `STEP_DELAY_MS` 
*  in plgTM1637.cpp
*
* 
* Connection:
* GND - GND - black
* VCC - +5V - red
* DIO - P0  - green
* CLK - P1  - blue
*
*/
/*
struct{
    
} plgTM1637Cfg;
*/
void plgTM1637();
