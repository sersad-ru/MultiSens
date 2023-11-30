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
#include "plgMHZ19.h"
#include "plgQRE1113.h"
#include "plgHCSR04.h"
#include "plgRDM6300.h"
#include "plgADXL345.h"

/*
 *
 * Еще акселерометр ADXL345 по I2C. 
 * Ну и нормально причесать код его
 * На закуску Ethernet в режима DHCP?
 * 
*/

// Registred plugins
MultiSensPlugin plugins[] = {
  {&plgFindPin,       "FindPin",          0},
  {&plgDigitalAnalog, "DigAn Read",       sizeof(plgDigitalAnalogCfg)},
  {&plgDigitalRead,   "DigitalRead",      sizeof(plgDigitalReadCfg)},
  {&plgAnalogRead,    "AnalogRead",       sizeof(plgAnalogReadCfg)},
  {&plgPWM,           "PWM",              sizeof(plgPWMCfg)},
  {&plgAM2302,        "AM2302/DHT22",     0},
  {&plgDS18B20,       "DS18B20",          0},
  {&plgIButton,       "iButton",          0},
  {&plgMHZ19,         "MH-Z19b",          0},
  {&plgQRE1113,       "QRE1113",          sizeof(plgQRE1113Cfg)},
  {&plgHCSR04,        "HC-SR04",          sizeof(plgHCSR04Cfg)},
  {&plgRDM6300,       "RDM-6300(125kHz)", 0},
  {&plgADXL345,       "ADXL345",          sizeof(plgADXL345)},
}; // 


void setup() {
  core.init(plugins, arraySize(plugins));
}// setup

void loop() {
  core.menu();
}//loop
