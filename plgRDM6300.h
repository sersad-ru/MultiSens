#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* RDM6300 plugin recieve .... 125kHz concentration from RDM6300.
* Connection:
* GND - GND - black
* VIN - +5V - red
* Tx - P0 - green  
* Rx - P1 - blue
*
* 
* Results are printed on the screen and sends to serial in human readable format.* 
* 
* * You can specify the delay between sensor calls using READ_DELAY_MS in plgMHZ19.cpp
*/

/*
struct{
    
} plgRDM6300Cfg;
*/
void plgRDM6300();
