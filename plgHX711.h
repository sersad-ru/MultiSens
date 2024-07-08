/*
Arduino Multi Senstor Tester
(c)2023-2024 by Sergey Sadovnikov (sersad@gmail.com)

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

#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* The HX711 plugin receives information from the HX711 Weighing Sensor and convert it to the weight in grams.
* 
* Remove any load from the sensor and press the SELECT button to set up zero weight.
* 
* To set up the conversion coefficient place the calibration sample (with known weight) to the sensor. 
* Current weight will be displayed. This value may be incorrect.
* Press the UP and DOWN buttons until the displayed value equals the known weight of the calibration sample.
* Press and hold the SELECT button to store the conversion coefficient in the EEPROM.
* 
* Connection:
* GND - GND - black
* VCC - +5V - red
* DT  - P0  - green 
* SCK - P1  - blue
*
*
*/

struct{
  int16_t coef; // Количество отсчетов на 1 грамм   
} plgHX711Cfg;

void plgHX711();
