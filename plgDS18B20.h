#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* DS18B20 plugin recieves temperature from the Dallas DS18B20 sensor.
* Connection:
* 1. GND - black
* 2. Data - P0 + 4k7 Ohm to VCC
* 3. VCC (+5V) - red

* 
* Results are printed on screen and sends to serial in human readable and SerialPlotter compartible format.
* 
* You can specify the delay between sensor calls using READ_DELAY_MS in plgDS18b20.cpp
*/

/*
struct{
    
} plgDS18B20Cfg;
*/
void plgDS18B20();