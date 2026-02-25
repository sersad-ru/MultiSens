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

#include "plgBME280.h"
#include <Wire.h>
#include "uBME280.h"

#define SDA_PIN P6 //Yellow-Black
#define SCL_PIN P7 //Gray-Black
#define READ_DELAY_MS 500 // 0.5 seconds between attempt

#define BME_ADDRESS 0x77 // BME280 I²C address

namespace BME280 {
  

} //namespace

using namespace BME280;
// == Main plugin function ==
void plgBME280(){
  // Init
  uint8_t need_header = 1;
  Wire.begin();   

  uBME280_Config bme; // BME Config

  if(!uBME280_Init(BME_ADDRESS, bme)) Serial.println(FF(MS_MSG_READ_ERROR));     
    
  // Main loop
  while(1){
    int16_t temp = uBME280_GetTemperature(BME_ADDRESS, bme);
    uint32_t pres = uBME280_GetPressure(BME_ADDRESS, bme);
    uint16_t hum = uBME280_GetHumidity(BME_ADDRESS, bme);

    core.printValScale(core, temp / 10); 
    core.print(MS_SYM_DEGREE_CODE);
    core.print(F("C "));
    core.printValScale(core, hum / 10);
    core.print(F("% "));
    core.print(pres / 133); 
    core.println(F("mmHg"));

    core.printValScale(Serial, temp / 10);
    Serial.print(", ");      
    core.printValScale(Serial, hum / 10);
    Serial.print(", ");                    
    Serial.println(pres / 133);

    if(need_header){
      Serial.print(F("Temperature(°C), Humidity(%), Pressure(mmHg), "));
      Serial.print(F("("));
      Serial.print(READ_DELAY_MS);
      Serial.println(FF(MS_MSG_DELAY_END));      
      need_header = 0;
    }//if
    delay(READ_DELAY_MS);
  }//while  
}//plgBME280
