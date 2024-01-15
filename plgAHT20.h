#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* AHT20 plugin recieve temperature and humidity from AHT20.
* Connection:
* GND - black* 
* VCC (+5V) - red
* SDA - P6 - yellow-black
* SCL - P7 - gray-black
* 
* Results are printed on screen and sends to serial in human readable and SerialPlotter compartible format.
* 
* You can specify the delay between sensor calls using READ_DELAY_MS in plgAHT20.cpp
* AHT20 I²C address in AHT_ADDRESS (plgAHT20.cpp)
* 
*/

/*
struct{
    
} plgAHT20Cfg;
*/
void plgAHT20();
