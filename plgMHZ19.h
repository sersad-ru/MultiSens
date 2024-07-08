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
* MHZ19 plugin recieves CO2 concentration from the MH-Z19b sensor.
* Connection:
* GND - GND - black
* VIN - +5V - red
* Tx - P0 - green  
* Rx - P1 - blue
*
* 
* Results are printed on the screen and sends to serial in human readable format.* 
* 
* * You can specify the delay between sensor calls using READ_DELAY_MS in plgMHZ19.cpp
*/

/*
struct{
    
} plgIMHZ19Cfg;
*/
void plgMHZ19();
