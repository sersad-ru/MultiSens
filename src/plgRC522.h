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
* The RC522 plugin recieves recieves 13.56MHz RFID card ID from the RFID RC522 sensor.
* Connection:
* GND  - Black 
* 3.3V - +3.3 - White
* SDA  - P0 - Green
* RST  - P1 - Blue
* MOSI - P2 - Yellow
* MISO - P3 - Violet
* SCK  - P4 - Brown
* 
* Results are printed on screen and sends to serial in human readable format.
* 
* You can specify the delay between sensor calls using READ_DELAY_MS in plgRC522.cpp
* 
*/

/*
struct{
    
} plgRC522Cfg;
*/
void plgRC522();
