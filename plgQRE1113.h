#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* QRE1113 plugin recieve distance information QRE1113 sensor.
* Connection:
* GND - GND - black
* VCC - +5V - red
* OUT - P0 - green  
*
* 
* Results are printed on the screen and sends to serial in human readable format.* 
*  
*/


struct{
  int8_t scan_mode;    
} plgQRE1113Cfg;

void plgQRE1113();
