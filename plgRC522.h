#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* The RC522 plugin recieves recieves 13.56MHz RFID card ID from the RFID RC522 sensor.
* Connection:
* GND  - Black 
* 3.3V - +3.3 - White
* SDA  - P0 - Green
* RST  - P1 - Blue
* MOSI - P2 - Yellow
* MISO - P3 - Violet
* SCK  - P4 - Brown
* 
* Results are printed on screen and sends to serial in human readable format.
* 
* You can specify the delay between sensor calls using READ_DELAY_MS in plgRC522.cpp
* 
*/

/*
struct{
    
} plgRC522Cfg;
*/
void plgRC522();
