#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* BMP280/AHT20 plugin receive temperature, humidity and pressure from BMP280+AHT20 module.
* Connection:
* GND - black* 
* VCC (+5V) - red
* SDA - P6 - yellow-black
* SCL - P7 - gray-black
* 
* Results are printed on screen and sends to serial in human readable and SerialPlotter compartible format.
* BMP280 used for pressure only. AHT20 for temperature and humidity.
* 
* You can specify the delay between sensor calls using READ_DELAY_MS in plgBMP280AHT20.cpp
* BMP280 I²C address in BMP_ADDRESS (plgBMP280AHT20.cpp)
* AHT20 I²C address in AHT_ADDRESS (plgBMP280AHT20.cpp)
* 
*/

/*
struct{
    
} plgBMP280AHT20Cfg;
*/
void plgBMP280AHT20();
