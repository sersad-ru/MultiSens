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

#include "plgI2CScan.h"
#include <Wire.h>

#define I2C_MAX_DEVICES 8 // Scan for first I2C_MAX_DEVICES devices
#define SDA_PIN P6 //Yellow-Black
#define SCL_PIN P7 //Gray-Black
#define I2C_SCAN_DELAY_MS 250 // 0.25 seconds between attempt

namespace I2CScan {
  const char plgI2C_NOT_FOUND[] PROGMEM = "No device(s)";
  const char plgI2C_0X[] PROGMEM = "0x";
}//namespace


using namespace I2CScan;
// == Main plugin function ==
void plgI2CScan(){
  // Init
  uint8_t devices[I2C_MAX_DEVICES]; // Device addresses
  uint8_t dev_cnt = 0; // Number of devices found
  
  core.moveCursor(0, 1); // First symbol of second line
  core.print(MS_SYM_SELECT_CODE);
  core.print(F("-start."));

  Wire.begin();

  core.moveCursor(0, 1);
  core.println();   

  for(uint8_t i = 8; i < 128; i++){
    Wire.beginTransmission(i);
    if(! Wire.endTransmission()){
      devices[dev_cnt++] = i;
      dev_cnt = min(dev_cnt, I2C_MAX_DEVICES - 1);
    }//if
    if(!(i & 7)) core.print(MS_SYM_PROGRESS_CODE); // i % 8 == 0
    delay(I2C_SCAN_DELAY_MS);
  }//for
  core.print(MS_SYM_PROGRESS_CODE);
    
  core.moveCursor(0, 1);
    
  core.println();   
  core.print(dev_cnt);
  core.print(F(": "));
  for(uint8_t i = 0; i < dev_cnt; i++){
    core.print(FF(plgI2C_0X));
    core.print(core.rAlign(devices[i], 2, '0', HEX));
    core.print(' ');
    Serial.print(FF(plgI2C_0X));
    Serial.println(core.rAlign(devices[i], 2, '0', HEX));
  }//for
  Serial.print(dev_cnt);
  Serial.println(F(" device(s) found."));

  if(!dev_cnt) core.println(FF(plgI2C_NOT_FOUND));

    
  // Main loop
  while(1){

  }//while  
}//plgI2CScan
