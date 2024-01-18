#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* iButton plugin recieves serial number the DS19xx iButton module.
* Connection:
* Ring - GND - black
* Center - Data - P0 + 2k2 (4k7 works too) Ohm to VCC
*
* 
* Results are printed on screen and sends to serial in human readable format. 
*/

/*
struct{
    
} plgIButtonCfg;
*/
void plgIButton();
