#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* The TSOP 312 plugin receives signals encoded with NEC protocol from an Infrared Remote Control Unit 
* using the TSOP312x receiver.
* 
* Results are decoded and displayed on the device screen and sends to the serial in human readable format. 
*
* 
* 
* Connection:
* GND - GND - black
* Vs  - +5V - red
* OUT - P0 - green
*
*/
/*
struct{
    
} plgTSOP312Cfg;
*/
void plgTSOP312();
