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
#include "plgFindPin.h"
#include "plgDigitalRead.h"
#include "plgAnalogRead.h"
#include "plgPWM.h"

/*
 * 
 * PWM (P1 + P2 + P3(soft? https://arduino.ru/forum/programmirovanie/analogwrite-na-lyubom-vyvode?ysclid=loukar7shp891808399))
 * https://alexgyver.ru/lessons/pwm-signal/
 * 
 * !!! Дописать soft PWM. Хватил ли 490 вызовов. НЕТ! И что там вообще будет. 
 * Короче, на простом цикле без прерываний выходит более-менее. Задержку подогнать и будет ок.
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
