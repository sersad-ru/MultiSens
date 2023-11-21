/*
* Arduino Multi Senstor Tester
* 
* v.1.0
* (c)2023 by sersad
* 
* 01.11.2023
* 09.11.2023
* 
* Arduino AVR Boards / Arduino UNO
*/

#include "mscore.h"
#include "plgFindPin.h"
#include "plgDigitalRead.h"
#include "plgAnalogRead.h"
#include "plgPWM.h"

/*
 *  AM3202 (DHT11)
 *  DS18B20 и следом таблетки dallas
 * 
*/

// Registred plugins
MultiSensPlugin plugins[] = {
  {&plgFindPin, "FindPin", 0},
  {&plgDigitalRead, "DigitalRead", sizeof(plgDigitalReadCfg)},
  {&plgAnalogRead, "AnalogRead", sizeof(plgAnalogReadCfg)},
  {&plgPWM, "PWM", sizeof(plgPWMCfg)},
}; // 


void setup() {
  core.init(plugins, arraySize(plugins));
}// setup

void loop() {
  core.menu();
}//loop
