#pragma once
#include <Arduino.h>
#include "mscore.h"

/*
* BME280 plugin recieves temperature, humidity and pressure from the BME280 sensor.
* Connection:
* GND - black* 
* VCC (+5V) - red
* SDA - P6 - yellow-black
* SCL - P7 - gray-black
* 
* Results are printed on screen and sends to serial in human readable and SerialPlotter compartible format.
* 
* You can specify the delay between sensor calls using READ_DELAY_MS in plgBME280.cpp
* BME280 I²C address in BME_ADDRESS (plgBME280.cpp)
* 
*/

/*
struct{
    
} plgBME280Cfg;
*/
void plgBME280();
