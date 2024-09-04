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

#include "plgBuzzer.h"

#define BUZZER_PIN P1 //Blue

namespace Buzzer {
} //namespace


using namespace Buzzer;

// == Main plugin function ==
void plgBuzzer(){
  // Init
/*  
  // Load settings from EEPROM 
  if(!core.loadSettings((uint8_t*)&plgBuzzerCfg)){
//    plgADXL345Cfg.offX = OFFSET_DEFAULT;// Settings was reseted. Use default values
    core.saveSettings((uint8_t*)&plgBuzzerCfg);// Save default value  
  }//if  
*/
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.println("Tone");
  digitalWrite(BUZZER_PIN, LOW);
  //analogWrite(BUZZER_PIN, 20);
  //tone(BUZZER_PIN, 4440);
  delay(500);
  Serial.println("NoTone");
  digitalWrite(BUZZER_PIN, HIGH);
  //analogWrite(BUZZER_PIN, 0);
  //noTone(BUZZER_PIN);
  
  // Init device
  // Main loop
  while(1){
  }//while    
}//plgBuzzer
