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
* The I2C Scanner plugin searches for first I2C_MAX_DEVICES (see plgI2CScan.cpp) devices on I2C bus.
* Connection:
* GND - GND - black
* VCC - +5V - red
* SDA - P6 - yellow-black
* SCL - P7 - gray-black
*/

void plgI2CScan();
