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
 * Сделть в ядре printWidth для чисел с заполнением слева нулями или пробелами. Мак ширина 10(11 если с минусом и 14 если с разделением пробеламы тысяч). Для DEC и HEX
 * Пишем digitalRead (P0 + P1), P0 - INPUT, P1 - INPUT_PULLUP. Прерывания: пр спаду и по фронту 
 * Потом analodRead (P5 + P6),
 * 
 * Для цифры выбор задержки опроса: int 100 250 500 1000 1500 2000 ms
 * Для аналога выбор задержки опроса: 100 250 500 1000 1500 2000 ms
 * 
 * и PWM (P1 + P2 + P3(soft? https://arduino.ru/forum/programmirovanie/analogwrite-na-lyubom-vyvode?ysclid=loukar7shp891808399))
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
