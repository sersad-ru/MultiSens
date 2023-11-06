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

/*
 * Меню и работа с EEPROM!
*/

MultiSensPlugin plugins[] = {{&pFindPin, "Find Pin"}}; //{{&plugin_b, "/"}, {&plugun_a, "/a"}}; void plugin_a(MultiSensCore& core)


void setup() {
  core.init(plugins, arraySize(plugins));
  
  core.println("The Quick");
  core.println("The Quick Brown Fox Junping Over The Lazy Dog");

}// setup

void loop() {

  MultiSensButton btn = core.getButton();
  Serial.println(btn);


}//loop
