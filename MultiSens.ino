/*
* Arduino Multi Senstor Tester
* 
* v.1.0
* (c)2023 by sersad
* 
* 01.11.2023
* 11.12.2023
* 09.01.2024
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
#include "plgWS2812.h"
#include "plgI2CScan.h"
#include "plgBME280.h"
#include "plgAHT20.h"
#include "plgBMP280AHT20.h"
#include "plgHTU21D.h"
#include "plgRC522.h"
#include "plgMPU6050.h"
#include "plgServo.h"

/*
 * Адресные ленты. Описать кнопки в хидере и прическать.
 * Серва с кондером и дописать
 * Шаговик и Серва
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
  {&plgRDM6300,       "RDM6300 (125kHz)", 0},
  {&plgADXL345,       "ADXL345",          sizeof(plgADXL345Cfg)},
  {&plgWS2812,        "WS2812b",          sizeof(plgWS2812Cfg)},
  {&plgI2CScan,       "I\1C Scanner",     0}, // custom symbol /1 is "²"
  {&plgBMP280AHT20,   "BMP280+AHT20",     0},
  {&plgBME280,        "BME280",           0},  
  {&plgAHT20,         "AHT20",            0},    
  {&plgHTU21D,        "HTU21D",           0},      
  {&plgRC522,         "RC522 (13.56MHz)", 0},      
  {&plgMPU6050,       "MPU6050",          0},
  {&plgServo,         "Servo",            sizeof(plgServoCfg)},
}; // 


void setup() {
  core.init(plugins, arraySize(plugins));
}// setup

void loop() {
  core.menu();
}//loop
