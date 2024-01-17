#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* HTU21D plugin recieve temperature and humidity from HTU21D.
* Connection:
* GND - black 
* VIN (+5V) - red
* SDA - P6 - yellow-black
* SCL - P7 - gray-black
* 
* Results are printed on screen and sends to serial in human readable and SerialPlotter compartible format.
* 
* You can specify the delay between sensor calls using READ_DELAY_MS in plgHTU21D.cpp
* HTU21D I²C address in HTU_ADDRESS (plgHTU21D.cpp)
* 
*/

/*
struct{
    
} plgHTU21DCfg;
*/
void plgHTU21D();
