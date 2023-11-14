#pragma once
#include <Arduino.h>
#include "mscore.h"

/*
* The DigitalRead plugin reads the digital pin. 
* P0 is configured as INPUT
* P1 is configured as INPUT_PULLP
*/

//
struct{
  int8_t scan_mode;
  
} pDigitalReadCfg;

void pDigitalRead(MultiSensCore& core);
