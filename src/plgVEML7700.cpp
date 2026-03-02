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

#include "plgVEML7700.h"
#include <Wire.h>


#define SDA_PIN P6 //Yellow-Black
#define SCL_PIN P7 //Gray-Black

#define READ_DELAY_MS 500 // 0.5 seconds between attempt

#define VEML_ADDRESS 0x10 // VEML7700 I²C address

namespace VEML7700 { 

#define VEML_CONF        0x00 // Configuration register
#define VEML_ALS_WH      0x01 // ALS high threshold window
#define VEML_ALS_WL      0x02 // ALS lowh threshold window
#define VEML_PWR         0x03 // Power saving register
#define VEML_ALS         0x04 // ALS data register
#define VEML_WHITE       0x05 // White data register
#define VEML_INT         0x06 // ALS INT trigger event
#define VEML_ID          0x07 // Device ID

#define VEML_ALS_GAIN    0x03 // ALS gain 1/4 
#define VEML_INTEGRATION 0x00 // Integration time 100ms
#define VEML_PERSISTENCE 0x00 // Presistence 1
#define VEML_INT_ENABLE  0x00 // Interrupt enable 0
#define VEML_SHUTDOWN    0x00 // Shutdown 0

 uint8_t _init(){
  // -- config 0x1800
  const uint16_t conf = (uint16_t)((VEML_ALS_GAIN << 11) | (VEML_INTEGRATION << 6) | (VEML_PERSISTENCE << 4) | (VEML_INT_ENABLE << 1) | VEML_SHUTDOWN);
  core.i2cWriteReg(VEML_ADDRESS, VEML_CONF, (uint16_t)(conf << 8) | (conf >> 8), SIZE_16); // swap hi-lo bytes

  // -- ALS_WH 0x0000
  core.i2cWriteReg(VEML_ADDRESS, VEML_ALS_WH, 0x0000, SIZE_16);

  // -- ALS_WL 0xFFFF
  core.i2cWriteReg(VEML_ADDRESS, VEML_ALS_WL, 0xFFFF, SIZE_16);

  // -- PWR 
  // power mode 3 = 10 (2-1)
  // power save disabled = 0 (0)
  //--- 0x0004
  core.i2cWriteReg(VEML_ADDRESS, VEML_PWR, 0x0400, SIZE_16);

  delay(3);
  return true;
 }//_init 

 uint16_t _read_val(){
  uint16_t raw = core.i2cReadReg(VEML_ADDRESS, VEML_ALS, SIZE_16);
  // swap hi-lo bytes
  float base = ((raw << 8) | (raw >> 8)) * 0.2688; // scale = 0.2688 (see Designing the VEML7700 Into an Application, page 5)  
  if(base <= 1000) return round(base); // No correction needed
  return round(base * (1.0023 + base *(8.1488e-5 + base * (-9.3924e-9 + base * 0.0135e-13))));
 }//_read_val
 

} //namespace


using namespace VEML7700;
// == Main plugin function ==
void plgVEML7700(){
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
}//plgVEML7700
