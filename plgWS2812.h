#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* WS2812 plugin can send commands to WE2812 LEDs. 
* ATTENTION! To prevent current overload don't use MultiSens on LED Strip. Only a few (2-3) leds can be used.
* 
* Connection:
* 1. VCC(+5V) - red
* 2. DI - -> 220 Ohm -> P0
* 3. GND - black
* 
*/

#define WS_LED_CNT 2 // LED count (2-3 max)

// WS2812 color space
typedef struct {
  uint8_t g;
  uint8_t r;
  uint8_t b;
} WS_LED;


struct{
  WS_LED leds[WS_LED_CNT];  
} plgWS2812Cfg;

void plgWS2812();
