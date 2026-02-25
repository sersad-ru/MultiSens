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

/*
* Отношение пинов MultiSens к пинам Adruino
* 
*/

const uint8_t MS_2_ARDUINO[] PROGMEM = {
  2,  // P0 Digital - INT0       - Green
  3,  // P1 Digital - INT1 - PWM - Blue
  11, // P2 Digital - MOSI - PWM - Yellow
  12, // P3 Digital - MISO       - Violet
  13, // P4 Digital - SCK        - Brown
  17, // P5 Analog               - Orange
  18, // P6 Analog  - SDA        - Yellow-Black
  19  // P7 Analog  - SCL        - Gray-Black 
};

#define P(x) pgm_read_byte(&MS_2_ARDUINO[x])

uint8_t _ms_getP(uint8_t aPin);
