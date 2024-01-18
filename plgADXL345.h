#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* ADXL345 plugin recieves acceleration on X, Y and Z axis from the ASXL345 accelerometer.
* Connection:
* GND - GND - black
* VCC - +5V - red
* SDA - P6 - yellow-black
* SCL - P7 - gray-black
*
* 
* Results are printed on the screen and sends to serial in human readable and SerialPlotter compartible format. 
* Use UP and DOWN buttons to display maximum and minimum values. Press UP/DOWN again to return to normal mode.
* Press SELECT to enter calibration mode. In this mode place sensor X-axis UP and press select, then repeat it for Y and Z axis.
* Press and hold SELECT button to store calibration offsets in EEPROM. It will be auto loaded and applied next time. 
* Hold DOWN button and pres RESET to clear stored value and return to defaults. (ATTENTION! All stored data will be cleared. FOR ALL plugins!)
* 
* You can specify the delay between sensor calls using READ_DELAY_MS in plgADXL345.cpp
*/


struct{
  int8_t offX; // offset X
  int8_t offY; // offset Y
  int8_t offZ; // offset Z           
} plgADXL345Cfg;

void plgADXL345();
