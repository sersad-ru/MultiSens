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

MultiSensCore core = MultiSensCore();


MultiSensPlugin plugins[] = {{&pFindPin, "Find Pin"}}; //{{&plugin_b, "/"}, {&plugun_a, "/a"}}; void plugin_a(MultiSensCore& core)

void setup() {
  core.init(plugins, sizeof(plugins) / sizeof(*plugins));
}// setup


void loop() {
}//loop
