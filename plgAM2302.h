#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* AM2302/DHT22 plugin recieves temperature and humidity from the AM2320(DHT22) sensor.
* Connection:
* 1. VCC(+5V) - red
* 2. Data - P0 + 10kOhm to VCC
* 3. NC
* 4. GND - black
* 
* Results are printed on screen and sends to serial in human readable and SerialPlotter compartible format.
* 
* You can specify the delay between sensor calls using READ_DELAY_MS in plgAM2302.cpp
*/

/*
struct{
    
} plgAM2302Cfg;
*/
void plgAM2302();
