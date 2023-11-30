#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* ADXL345 plugin recieve acceleration on X, Y and Z axis from the ASXL345 accelerometer.
* Connection:
* GND - GND - black
* VCC - +5V - red
* SDA - P6 - yellow-black
* SCL - P7 - gray-black
*
* 
* Results are printed on the screen and sends to serial in human readable and SerialPlotter compartible format. 
* 
* You can specify the delay between sensor calls using READ_DELAY_MS in plgADXL345.cpp
*/


struct{
  uint8_t val;    
} plgADXL345Cfg;

void plgADXL345();
