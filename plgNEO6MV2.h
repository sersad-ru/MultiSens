#pragma once
#include <Arduino.h>
#include "mscore.h" 

/*
* The GY-NEO6MV2 plugin receives information from ...
*
* Connection:
* GND - GND - black
* VCC - +5V - red
* TX  - P0 - green  
* RX  - P1 - blue
* 
*/

/* The main plugin function declaration */
void plgNEO6MV2();
