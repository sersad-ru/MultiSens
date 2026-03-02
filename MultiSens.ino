/*
* Arduino Multi Sensor Tester
* 
* v.1.1
* (c)2023-2025 by Sergey Sadovnikov (sersad@gmail.com)
* 
* 01.11.2023
* 11.12.2023
* 09.01.2024
* 08.07.2024
* 29.04.2025
* 24.02.2026
* 
* Arduino AVR Boards / Arduino UNO
*/

/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <www.gnu.org/licenses/>.
*/

#include "src/mscore.h"
#include "src/plgFindPin.h"
#include "src/plgDigitalAnalog.h"
#include "src/plgDigitalRead.h"
#include "src/plgAnalogRead.h"
#include "src/plgPWM.h"
#include "src/plgAM2302.h"
#include "src/plgDS18B20.h"
#include "src/plgIButton.h"
#include "src/plgMHZ19.h"
#include "src/plgQRE1113.h"
#include "src/plgHCSR04.h"
#include "src/plgRDM6300.h"
#include "src/plgADXL345.h"
#include "src/plgWS2812.h"
#include "src/plgI2CScan.h"
#include "src/plgBME280.h"
#include "src/plgAHT20.h"
#include "src/plgBMP280AHT20.h"
#include "src/plgHTU21D.h"
#include "src/plgRC522.h"
#include "src/plgMPU6050.h"
#include "src/plgServo.h"
#include "src/plgStepper.h"
#include "src/plgTSOP312.h"
#include "src/plgREncoder.h"
#include "src/plgHX711.h"
#include "src/plgW5500Lite.h"
#include "src/plgHC06.h"
#include "src/plgNEO6MV2.h"
#include "src/plgModBusScan.h"
#include "src/plgTM1637.h"
#include "src/plgBuzzer.h"
#include "src/plgMAX9814.h"
#include "src/plgBH1750.h"
#include "src/plgVEML7700.h"
#include "src/plgSample.h"


// Registred plugins
MultiSensPlugin plugins[] = {

  {&plgFindPin,       "FindPin",          0},
  {&plgDigitalAnalog, "DigAn Read",       sizeof(plgDigitalAnalogCfg)}, 
  //{&plgDigitalRead,   "DigitalRead",      sizeof(plgDigitalReadCfg)},
  //{&plgAnalogRead,    "AnalogRead",       sizeof(plgAnalogReadCfg)},
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
  {&plgStepper,       "Stepper",          sizeof(plgStepperCfg)},
  {&plgTSOP312,       "TSOP312",          0},
  {&plgHC06,          "HC-05/06",         0},  

  //{&plgREncoder,      "Encoder",          0}, 
  //{&plgW5500Lite,     "W5500 Lite",       0},  
  //{&plgHX711,         "HX711",          sizeof(plgHX711Cfg)}, 
  //{&plgNEO6MV2,       "GY-NEO6MV2",       0},  
  //{&plgModBusScan,    "ModBus Scanner",   0},
  //{&plgTM1637,        "TM1637",           0},
  //{&plgBuzzer,        "Buzzer",           sizeof(plgBuzzerCfg)},
  //{&plgMAX9814,       "MAX9814",          0},
  //{&plgBH1750,        "BH1750",           0},
  //{&plgVEML7700,      "VEML7700",         0},
  //{&plgSample,        "Sample Plugin",  0},   
}; // 


void setup() {
  core.init(plugins, arraySize(plugins));
}// setup

void loop() {
  core.menu();
}//loop
