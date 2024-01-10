#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* BMP280/AHT20 plugin recieve temperature, humidity and pressure BMP280+AHT20 module.
* Connection:
* GND - black* 
* VCC (+5V) - red
* SDA - P6 - yellow-black
* SCL - P7 - gray-black
* 
* Results are printed on screen and sends to serial in human readable and SerialPlotter compartible format.
* 
* You can specify the delay between sensor calls using READ_DELAY_MS in plgBMP280AHT20.cpp
*/

/*
struct{
    
} plgBMP280AHT20Cfg;
*/
void plgBMP280AHT20();
