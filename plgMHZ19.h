#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* MHZ19 plugin recieves CO2 concentration from the MH-Z19b sensor.
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
    
} plgIMHZ19Cfg;
*/
void plgMHZ19();
