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

#include "plgHCSR04.h"

#define TRIG_PIN P0
#define ECHO_PIN P1

#define READ_DELAY_MS 500 // 0.5 seconds betwen reading
#define DEFAULT_TEMP 25 // Default temperature 25

namespace HCSR04 {

uint16_t _read(const int8_t temp){
  //Send a pulse
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  //Receive the echo, calculate and return the distanse in mm (Thank's to Gyver: https://kit.alexgyver.ru/tutorials/hc-sr04/)
  return ((pulseIn(ECHO_PIN, HIGH) * ((temp * 6) / 10 + 331)) / 2000);  
}//_read


uint16_t _read_filtered(const int8_t temp){
  static int16_t res = 0;
  int16_t val = _read(temp);
  if(abs(res - val) > res >> 2) res = val; // 4% tolerance (res >> 2)
  return res;
}//_read_filtered

}//namespace

using namespace HCSR04;
  
// == Main plugin function ==
void plgHCSR04(){
  // Init
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Load settings from EEPROM 
  if(!core.loadSettings((uint8_t*)&plgHCSR04Cfg)){
    plgHCSR04Cfg.temp = DEFAULT_TEMP;// Settings was reseted. Use default values
    core.saveSettings((uint8_t*)&plgHCSR04Cfg);// Save default value  
  }//if  
  
  Serial.print(F("Distance(mm), Temperature(°C), ("));
  Serial.print(READ_DELAY_MS);
  Serial.println(FF(MS_MSG_DELAY_END));


  // Main loop
  while(1){
    // Process user input    
    switch (core.getButton()) {
      case UP:
      case UP_LONG: plgHCSR04Cfg.temp++; break;
      
      case DOWN:
      case DOWN_LONG: plgHCSR04Cfg.temp--; break;
        
      case SELECT_LONG: core.saveSettings((uint8_t*)&plgHCSR04Cfg);break;   // save settings to EEPROM
      
      default: break;
    }//switch

    plgHCSR04Cfg.temp = constrain(plgHCSR04Cfg.temp, -50, 50); // -50..50
  
    uint16_t dist = _read_filtered(plgHCSR04Cfg.temp);
    core.moveCursor(0, 1); 
    core.print(F("D:"));
    core.print(core.rAlign(dist, 4));
    core.print(F("mm"));
    core.print(F(" T:"));
    core.print(plgHCSR04Cfg.temp);
    core.print(MS_SYM_DEGREE_CODE);
    core.println(F("C"));

    Serial.print(dist);
    Serial.print(F(", "));
    Serial.print(plgHCSR04Cfg.temp);
    Serial.print(F(" ("));
    Serial.print(READ_DELAY_MS);
    Serial.println(FF(MS_MSG_DELAY_END));

    delay(READ_DELAY_MS);    
  }//while  
}//plgHCSR04
