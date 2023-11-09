#pragma once
#include <Arduino.h>
#include "mscore.h"

/*
* The DigitalRead plugin reads the digital pin. 
*/

//
struct{
  uint8_t a;
  
} pDigitalReadCfg;

void pDigitalRead(MultiSensCore& core);
