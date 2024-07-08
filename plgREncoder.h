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
* The REncoder plugin receives signals from the rotary encoder and displays the rotating direction and the button status. 
* When encoder rotated clockwise (CW) it increase the counter. 
* In other direction (CCW) the counter decrease. 
* If the button is pressed, the counter will be resetted to zero.
*
* Don't forget to add pull-up resistors if you are using a standalone encoder (not the preassembled module). 
* 
* Results are displayed on the device screen and sends to the serial in human readable format. 
* 
* 
* Connection:
* GND - GND - black
* VCC - +5V - red
* Key - P0  - green 
* S1  - P1  - blue
* S2  - P2  - yellow 
*
*
*/
/*
struct{
    
} plgREncoderCfg;
*/
void plgREncoder();
