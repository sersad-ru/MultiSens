#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* WS2812 plugin can srnd commands to WE2812 LEDs. 
* ATTENTION! To prevent current overload don't use MultiSens on LED Strip. Only a few (2-3) leds can be used.
* 
* Connection:
* 1. VCC(+5V) - red
* 2. DI - -> 220 Ohm -> P0
* 3. GND - black
* 
*/


struct{
  uint8_t data;  
} plgWS2812Cfg;

void plgWS2812();
