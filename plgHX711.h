#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* The HX711 plugin recieves singnals from the HX711 Weighing sensor.
* 
* Select - to zero (tare function)
* UP/DOWN - correct scale coefficient.
* 
* Connection:
* GND - GND - black
* VCC - +5V - red
* DT  - P0  - green 
* SCK - P1  - blue
*
*
*/

struct{
  int16_t coef; // Количество отсчетов на 1 грамм   
} plgHX711Cfg;

void plgHX711();
