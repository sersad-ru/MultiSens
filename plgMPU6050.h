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
