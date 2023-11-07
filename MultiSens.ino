/*
* Arduino Multi Senstor Tester
* 
* v.1.0
* (c)2023 by sersad
* 
* 01.11.2023
*/

#include "mscore.h"
#include "pFindPin.h"
#include "pDigitalRead.h"

/*
 * Работа с EEPROM!
 * Интерфейс чтения/записи EEPROM для плагинов. + резервирование EEPROM при регистрации плагина
*/

MultiSensPlugin plugins[] = {
  {&pFindPin, "Find Pin"},
  {&pDigitalRead, "Digital Read"}
}; // void plugin_a(MultiSensCore& core)


void setup() {
  core.init(plugins, arraySize(plugins));
}// setup

void loop() {
  core.menu();
/*
  MultiSensButton btn = core.getButton();
  Serial.println(btn);
*/

}//loop
