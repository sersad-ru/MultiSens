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
#include "pAnalogRead.h"

/*
 * 
 * PWM (P1 + P2 + P3(soft? https://arduino.ru/forum/programmirovanie/analogwrite-na-lyubom-vyvode?ysclid=loukar7shp891808399))
 * https://alexgyver.ru/lessons/pwm-signal/
 * 
*/

// Registred plugins
MultiSensPlugin plugins[] = {
  {&pFindPin, "FindPin", 0},
  {&pDigitalRead, "DigitalRead", sizeof(pDigitalReadCfg)},
  {&pAnalogRead, "AnalogRead", sizeof(pAnalogReadCfg)},
}; // 


void setup() {
  core.init(plugins, arraySize(plugins));
}// setup

void loop() {
  core.menu();
}//loop
