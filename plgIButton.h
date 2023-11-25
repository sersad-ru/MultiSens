#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* iButton plugin recieve serial number DS18B20 iButtonr.
* Connection:
* Ring - GND - black
* Center - Data - P0 + 2k2 (4k7 works too) Ohm to VCC
*
* 
* Results are printed on screen and sends to serial in human readable format.* 
*/

/*
struct{
    
} plgIButtonCfg;
*/
void plgIButton();
