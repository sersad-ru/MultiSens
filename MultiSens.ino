/*
* Arduino Multi Senstor Tester
* 
* v.1.0
* (c)2023 by sersad
* 
* 01.11.2023
* 09.11.2023
*/

#include "mscore.h"
#include "pFindPin.h"
#include "pDigitalRead.h"

/*
 * Пишем digitalRead,
 * Потом analodRead,
 * и PWM
*/

// Registred plugins
MultiSensPlugin plugins[] = {
  {&pFindPin, "Find Pin", 0},
  {&pDigitalRead, "Digital Read", sizeof(pDigitalReadCfg)}
}; // 


void setup() {
  core.init(plugins, arraySize(plugins));
}// setup

void loop() {
  core.menu();
}//loop
