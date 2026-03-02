/*
Arduino Multi Senstor Tester
(c)2023-2026 by Sergey Sadovnikov (sersad@gmail.com)

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

#include "plgBH1750.h"
#include <Wire.h>


#define SDA_PIN P6 //Yellow-Black
#define SCL_PIN P7 //Gray-Black

#define READ_DELAY_MS 500 // 0.5 seconds between attempt (sensor needs about 120ms)

#define BH1750_ADDRESS 0x23 // BH1750 I²C address (0x5C - if ADD > 0.7 * VCC)

namespace BH1750 { 

#define BH1750_MODE       0x10 // Continious Hi-Res mode
#define BH1750_MT_REG     69   // MTReg value (31-254)
#define BH1750_MT_REG_DEF 69   // Default MTReg value (31-254)
#define BH1750_SCALE_X100 120  // Scale factor x100 (1.2 * 100 = 120)

 uint8_t _init(){
  // Пишем MTReg. Старшие биты 01000_MT[7,6,5], младшие: 011_MT[4,3,2,1,0]
  Wire.beginTransmission(BH1750_ADDRESS);
  Wire.write(0x40 | (BH1750_MT_REG >> 5)); // Старшие 3
  Wire.endTransmission();

  Wire.beginTransmission(BH1750_ADDRESS);
  Wire.write(0x60 | (BH1750_MT_REG & 0x1F)); // Младшие 5
  Wire.endTransmission();

  // Пишем режим
  Wire.beginTransmission(BH1750_ADDRESS);  
  Wire.write(BH1750_MODE);
  uint8_t res = Wire.endTransmission();
  delay(10);
  return !res;
 }//_init 


 uint16_t _read_val(){
  Wire.requestFrom(BH1750_ADDRESS, 2);
  uint32_t res = Wire.read() << 8;
  res |= Wire.read();

  return ((res * 100) * (uint32_t)BH1750_MT_REG_DEF) / ((uint32_t)BH1750_MT_REG * (uint32_t)BH1750_SCALE_X100);
 }//_read_val
} //namespace


using namespace BH1750;
// == Main plugin function ==
void plgBH1750(){
  // Init
  Wire.begin(); 

  if(!_init()) Serial.println(FF(MS_MSG_READ_ERROR));
  else {
    core.moveCursor(0, 1);
    core.print(F("Light: "));
  }//if

  uint16_t lux = 0;

  // Main loop
  while(1){
    core.moveCursor(7, 1);

    lux = _read_val();
    core.print(core.rAlign(lux, 5));
    core.println(F(" lux"));
    Serial.print(F("Light(lux):"));
    Serial.println(lux);

    delay(READ_DELAY_MS);    
  }//while  
}//plgBH1750
