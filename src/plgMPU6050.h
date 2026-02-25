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
* MPU6050 plugin recieves the acceleration on X, Y and Z axis, the rotation around axes and 
the temperature from the MPU6050 sensor.

* Connection:
* GND - GND - black
* VCC - +5V - red
* SDA - P6 - yellow-black
* SCL - P7 - gray-black
*
* 
* Results are printed on the screen and sends to serial in human readable and SerialPlotter compartible format. 
* 
* X, Y and Z show the acceleration along x, y, and z axes. Values range from -2 to +2 G.
* A, B and C show the rotation around x, y and z axes. In degrees/s. Values range from -250 to 250.
* T shows the current temperature in °C
* 
* You can specify the delay between sensor calls using READ_DELAY_MS in plgMPU6050.cpp
* MPU6050 I²C address in MPU_ADDRESS (plgMPU6050.cpp)
*/

/*
struct{
} plgMPU6050Cfg;
*/
void plgMPU6050();
