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
* iButton plugin recieves serial number the DS19xx iButton module.
* Connection:
* Ring - GND - black
* Center - Data - P0 + 2k2 (4k7 works too) Ohm to VCC
*
* 
* Results are printed on screen and sends to serial in human readable format. 
*/

/*
struct{
    
} plgIButtonCfg;
*/
void plgIButton();
