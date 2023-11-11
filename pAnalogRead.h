#pragma once
#include <Arduino.h>
#include "mscore.h"

/*
* The AnalogRead plugin ...
*/

struct{
  uint8_t scan_mode;
  
} pAnalogReadCfg;

void pAnalogRead(MultiSensCore& core);
