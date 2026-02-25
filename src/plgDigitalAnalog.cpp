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

#include "plgDigitalAnalog.h"

#define DIG_PIN P0
#define AN_PIN P5

#define DEFAULT_SCAN_MODE 4 //Use 500ms delay as default. See MS_STD_DELAYS in mscore.h 

namespace DigitalAnalog {

void _process_pin(int16_t new_val, uint8_t offset, int16_t &oldVal, uint8_t is_analog){    
  //to serial
  if(is_analog) Serial.print(new_val);
   else Serial.print(new_val * 1023); // scale for Serial plot
  
  if(new_val == oldVal) return; // No changes

  //to display  
  core.moveCursor(offset, 1);
  if(is_analog) core.print(core.rAlign(new_val, 4));
    else core.print(new_val);

  //save time
  oldVal = new_val;
}//_process_pin

}//namespace

using namespace DigitalAnalog;


// == Main plugin function ==
void plgDigitalAnalog(){
  // Init 
  pinMode(DIG_PIN, INPUT);
    
  // Load settings from EEPROM 
  if(!core.loadSettings((uint8_t*)&plgDigitalAnalogCfg)){
    plgDigitalAnalogCfg.scan_mode = DEFAULT_SCAN_MODE;// Settings was reseted. Use default values
    core.saveSettings((uint8_t*)&plgDigitalAnalogCfg);// Save default value  
  }//if  

  uint16_t cur_delay = pgm_read_word(&MS_STD_DELAYS[plgDigitalAnalogCfg.scan_mode]);

  // Dispaly init
  core.moveCursor(0, 1); // First symbol of second line
  core.print(core.getPinName(DIG_PIN));
  core.moveCursor(5, 1);
  core.print(core.getPinName(AN_PIN));
  core.moveCursor(13, 1);
  core.print(FF(MS_MSG_DELAY_NAME));

  int16_t oldDig = -1;
  int16_t oldAn = -1;
  int8_t old_mode = -1;
  
  // Main loop
  while(1){
    // Process user input    
    switch (core.getButton()) {
      case UP:
      case UP_LONG: plgDigitalAnalogCfg.scan_mode++; break;
      
      case DOWN:
      case DOWN_LONG: plgDigitalAnalogCfg.scan_mode--; break;
        
      case SELECT_LONG: core.saveSettings((uint8_t*)&plgDigitalAnalogCfg); break;   // save settings to EEPROM
      
      default: break;
    }//switch

    plgDigitalAnalogCfg.scan_mode = constrain(plgDigitalAnalogCfg.scan_mode, 0, (int8_t)arraySize(MS_STD_DELAYS) - 1);

    // was scan mode changed?
    if(old_mode != plgDigitalAnalogCfg.scan_mode){
      //Scan mode was changed
      old_mode = plgDigitalAnalogCfg.scan_mode;
      core.moveCursor(15, 1);
      // delay mode
      cur_delay = pgm_read_word(&MS_STD_DELAYS[plgDigitalAnalogCfg.scan_mode]);
      core.println(cur_delay);
      Serial.print(F("P0, P5 ("));
      Serial.print(cur_delay);
      Serial.println(FF(MS_MSG_DELAY_END));   
    }//if

    // Scan Mode pin values
    _process_pin(digitalRead(DIG_PIN), 3, oldDig, false);
    Serial.print(", ");
    _process_pin(analogRead(AN_PIN), 8, oldAn, true);
    Serial.println();
    delay(cur_delay);    
  }//while
}//plgDigitalAnalog
