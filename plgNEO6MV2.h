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
* The NEO6MV2 plugin receives information from the NEO6MV2 GPS receiver module.
*
* Results are displayed on the device screen and sends to the `Serial` in human-readable format. 
* 
* This plugin can parse `GGA`, `RMC`, `GSV` and `ZDA` GPS messages. Information from this messages
* are decoded and collected and then displayed in human-readable format. 
*
* Connect the module and run the plugin.
* The MultiSens will try to find the module. 
* Decoded information will be dispayed every `DISPLAY_DELAY_MS` milliseconds (see [plgNEO6MV2.cpp])
* Press the `UP` and `DOWN` keys to switch information on the screen.
* Press the `SELECT` key to turn on/off sending raw GPS data to the `Serial`.
*
* There are four display modes.
* 1. The latitude. The first symbol can be `A` (for correct data) or `V`(for incorrect). The second 
*    -symbol is the GPS quality indicator. **0** - fix not available, **1** - GPS fix, **2** - Differential GPS fix.
*    The next value is the direction `N` for North and `S` for South. Then the current latitude is displayed.
*
* 2. The longitude. The first symbol can be `A` (for correct data) or `V`(for incorrect). The second 
*    symbol is the GPS quality indicator. **0** - fix not available, **1** - GPS fix, **2** - Differential GPS fix.
*    The next value is the direction `E` for East and `W` for West. Then the current longitude is displayed.
*
* 3. The sattelites. The name of the positioning system, the number of the currently active sattelites,
*    the number of total visible sattelites, `A` (for correct data) or `V`(for incorrect) and the GPS quality indicator.
*
* 4. The data and time. The date and time (in UTC) received from the module.
*
*
* Connection:
* GND - GND - black
* VCC - +5V - red
* TX  - P0 - green  
* RX  - P1 - blue
* 
*/

/* The main plugin function declaration */
void plgNEO6MV2();
