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
#define BUZZER_DEFAULT_HZ 440
namespace Buzzer {
  const char BUZZER_HZ_MSG[] PROGMEM = " Hz";    
  const char BUZZER_PLAY_MSG[] PROGMEM = "|> Freq: ";    
  const char BUZZER_STOP_MSG[] PROGMEM = "|| Freq: ";    

  void _disp(const uint16_t freq, const uint8_t state){
    core.moveCursor(0, 1);     
    if(state){ 
      core.print(FF(BUZZER_PLAY_MSG));
      Serial.print(FF(BUZZER_PLAY_MSG));
    }//if
    else {
      core.print(FF(BUZZER_STOP_MSG));
      Serial.print(FF(BUZZER_STOP_MSG));
    }//if..else
   
    core.print(freq); 
    core.println(FF(BUZZER_HZ_MSG));
    
    Serial.print(freq);
    Serial.println(FF(BUZZER_HZ_MSG));     
  }//_disp
} //namespace


using namespace Buzzer;

// == Main plugin function ==
void plgBuzzer(){
  // Init

  // Load settings from EEPROM 
  if(!core.loadSettings((uint8_t*)&plgBuzzerCfg)){
    plgBuzzerCfg.freq = BUZZER_DEFAULT_HZ;// Settings was reseted. Use default values
    core.saveSettings((uint8_t*)&plgBuzzerCfg);// Save default value  
  }//if  

  // Dispaly init
 
  uint16_t freq_old = plgBuzzerCfg.freq;
  uint8_t bz_play = false;
  uint8_t bz_play_old = bz_play;
  
  _disp(plgBuzzerCfg.freq, bz_play);
  
  // Main loop
  while(1){
    switch (core.getButton()) {
      case UP: plgBuzzerCfg.freq++; break;
      case UP_LONG: plgBuzzerCfg.freq+=10; break;
      
      case DOWN: plgBuzzerCfg.freq--; break;
      case DOWN_LONG: plgBuzzerCfg.freq-=10; break;
      
      case SELECT:  bz_play = !bz_play; break;    
      
      case SELECT_LONG: core.saveSettings((uint8_t*)&plgBuzzerCfg); break;   // save settings to EEPROM   
      default: break;
    }//switch 
    plgBuzzerCfg.freq = constrain(plgBuzzerCfg.freq, 31, 8000); // https://github.com/bhagman/Tone#ugly-details
    if((bz_play == bz_play_old) && (freq_old == plgBuzzerCfg.freq)) continue;
    
    bz_play_old = bz_play;
    freq_old = plgBuzzerCfg.freq;
    
    if(bz_play) tone(BUZZER_PIN, plgBuzzerCfg.freq);
      else noTone(BUZZER_PIN);

    _disp(plgBuzzerCfg.freq, bz_play);
  }//while    
}//plgBuzzer
