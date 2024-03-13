#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* The W5500Lite plugin ...
* 
* Connection:
* GND  - GND   - black
* VCC  - +3.3V - white
* CS   - P0    - green 
* MOSI - P2    - yellow
* MISO - P3    - violet
* SCK  - P4    - brown
*
*/
/*
struct{
  uint8_t data;  
} plgW5500LiteCfg;
*/
void plgW5500Lite();
