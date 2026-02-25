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

#include "plgQRE1113.h"

#define WORK_PIN P0
#define TIMEOUT_MS 3000 //Sensor timeout
#define DEFAULT_SCAN_MODE 4 //Use 500ms delay as default. See MS_STD_DELAYS in mscore.h

namespace QRE1113 {

const char plgQRE_TOO_LONG[] PROGMEM = "Too long distance";

uint16_t _read(){
  pinMode(WORK_PIN, OUTPUT);
  digitalWrite(WORK_PIN, HIGH);
  delayMicroseconds(10);
  pinMode(WORK_PIN, INPUT);

  uint32_t t = micros();
  while(digitalRead(WORK_PIN) && ((micros() - t) < TIMEOUT_MS));
  return micros() - t;
}//_read

} // namespace

using namespace QRE1113;

// == Main plugin function ==
void plgQRE1113(){
  // Init
 
  // Load settings from EEPROM 
  if(!core.loadSettings((uint8_t*)&plgQRE1113Cfg)){
    plgQRE1113Cfg.scan_mode = DEFAULT_SCAN_MODE;// Settings was reseted. Use default values
    core.saveSettings((uint8_t*)&plgQRE1113Cfg);// Save default value  
  }//if  

  uint16_t cur_delay = pgm_read_word(&MS_STD_DELAYS[plgQRE1113Cfg.scan_mode]);
  int8_t old_mode = -1;
  uint8_t need_header = 1;
      
  // Main loop
  while(1){
    // Process user input    
    switch (core.getButton()) {
      case UP:
      case UP_LONG: plgQRE1113Cfg.scan_mode++; break;
      
      case DOWN:
      case DOWN_LONG: plgQRE1113Cfg.scan_mode--; break;
        
      case SELECT_LONG: core.saveSettings((uint8_t*)&plgQRE1113Cfg);break;   // save settings to EEPROM
      
      default: break;
    }//switch

    plgQRE1113Cfg.scan_mode = constrain(plgQRE1113Cfg.scan_mode, 0, (int8_t)arraySize(MS_STD_DELAYS) - 1);

    // was scan mode changed?
    if(old_mode != plgQRE1113Cfg.scan_mode){
      //Scan mode was changed
      old_mode = plgQRE1113Cfg.scan_mode;
      cur_delay = pgm_read_word(&MS_STD_DELAYS[plgQRE1113Cfg.scan_mode]);
      need_header = 1;
    }//if

    uint16_t val = _read(); // Read data from the sensor
    
    core.moveCursor(0, 1);
    core.print(F("Val:"));
    core.print(core.rAlign(val, 4));
    core.print(F("  D:"));
    core.print(core.rAlign(cur_delay, 4));

    if(need_header){ // Need to print header for serial plotter
      Serial.print(F("Value, ("));
      Serial.print(cur_delay);
      Serial.println(FF(MS_MSG_DELAY_END));
      need_header = 0;
    }//if

    Serial.println(val);
    
    delay(cur_delay);  
  }//while  
}//plgQRE1113
