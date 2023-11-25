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
#include "plgDigitalAnalog.h"
#include "plgDigitalRead.h"
#include "plgAnalogRead.h"
#include "plgPWM.h"
#include "plgAM2302.h"
#include "plgDS18B20.h"
#include "plgIButton.h"

/*
 * Lfnxbr CO2 по Serial
 * Дальше сенсоры расстояния
 * Еще акселерометр по I2C
 * На закуску Ethernet в режима DHCP?
 * 
*/

// Registred plugins
MultiSensPlugin plugins[] = {
  {&plgFindPin, "FindPin", 0},
  {&plgDigitalAnalog, "DigAn Read", sizeof(plgDigitalAnalogCfg)},
  {&plgDigitalRead, "DigitalRead", sizeof(plgDigitalReadCfg)},
  {&plgAnalogRead, "AnalogRead", sizeof(plgAnalogReadCfg)},
  {&plgPWM, "PWM", sizeof(plgPWMCfg)},
  {&plgAM2302, "AM2302/DHT22", 0},
  {&plgDS18B20, "DS18B20", 0},
  {&plgIButton, "iButton", 0},
}; // 


void setup() {
  core.init(plugins, arraySize(plugins));
}// setup

void loop() {
  core.menu();
}//loop
