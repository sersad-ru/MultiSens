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

#include "plgDigitalRead.h"

#define INPUT_PIN P0
#define INPUT_PULLUP_PIN P1

#define DEFAULT_SCAN_MODE 4 //Use 500ms delay as default. See MS_STD_DELAYS in mscore.h

namespace DigitalRead {

// Pin values for interrupt mode
volatile uint8_t newP0 = 0;
volatile uint8_t newP1 = 1;

// P0 interrupt service routine
void _p0_isr(){
  newP0 = digitalRead(INPUT_PIN);
}//_p0_isr


// P1 interrupt service routine
void _p1_isr(){
  newP1 = digitalRead(INPUT_PULLUP_PIN);
}//_p1_isr


void _process_pin(uint8_t new_val, uint8_t offset, uint8_t &oldVal){
  //to serial
  Serial.print(new_val);
  
  if(new_val == oldVal) return; // No changes

  //to display  
  core.moveCursor(offset, 1);
  core.print(new_val);

  //save time
  oldVal = new_val;
}//_process_pin

} // namespace

using namespace DigitalRead;

// == Main plugin function ==
void plgDigitalRead(){
  // Init    
  pinMode(INPUT_PIN, INPUT);
  pinMode(INPUT_PULLUP_PIN, INPUT_PULLUP);

  // Load settings from EEPROM 
  if(!core.loadSettings((uint8_t*)&plgDigitalReadCfg)){
    plgDigitalReadCfg.scan_mode = DEFAULT_SCAN_MODE;// Settings was reseted. Use default values
    core.saveSettings((uint8_t*)&plgDigitalReadCfg);// Save default value  
  }//if  

  uint16_t cur_delay = pgm_read_word(&MS_STD_DELAYS[plgDigitalReadCfg.scan_mode]);
  
  // Dispaly init
  core.moveCursor(0, 1); // First symbol of second line
  core.print(core.getPinName(INPUT_PIN));
  core.moveCursor(5, 1);
  core.print(core.getPinName(INPUT_PULLUP_PIN));
  core.moveCursor(10, 1);
  core.print(FF(MS_MSG_DELAY_NAME));

  uint8_t oldP0 = 2;
  uint8_t oldP1 = 2;
  uint32_t timeP0 = millis();
  uint32_t timeP1 = millis();
  int8_t old_mode = -2;
  
  // Main loop
  while(1){
    // Process user input    
    switch (core.getButton()) {
      case UP:
      case UP_LONG: plgDigitalReadCfg.scan_mode++; break;
      
      case DOWN:
      case DOWN_LONG: plgDigitalReadCfg.scan_mode--; break;
        
      case SELECT_LONG: core.saveSettings((uint8_t*)&plgDigitalReadCfg); break; // save settings to EEPROM
      
      default: break;
    }//switch

    plgDigitalReadCfg.scan_mode = constrain(plgDigitalReadCfg.scan_mode, -1, (int8_t)arraySize(MS_STD_DELAYS) - 1);
    
    // scan mode was changed?
    if(old_mode != plgDigitalReadCfg.scan_mode){
      //Scan mode was changed
      old_mode = plgDigitalReadCfg.scan_mode;
      core.moveCursor(12, 1);
      if(plgDigitalReadCfg.scan_mode == -1){ // Interrupt mode
        core.println(F("INT"));
        Serial.println(F("P0, P1 (Interrup mode.)"));        
        attachInterrupt(digitalPinToInterrupt(INPUT_PIN), _p0_isr, CHANGE);
        attachInterrupt(digitalPinToInterrupt(INPUT_PULLUP_PIN), _p1_isr, CHANGE);
        continue;
      }//if     
      // delay mode
      detachInterrupt(digitalPinToInterrupt(INPUT_PIN));
      detachInterrupt(digitalPinToInterrupt(INPUT_PULLUP_PIN));
      cur_delay = pgm_read_word(&MS_STD_DELAYS[plgDigitalReadCfg.scan_mode]);
      core.println(cur_delay);
      Serial.print(F("P0, P1 ("));
      Serial.print(cur_delay);
      Serial.println(FF(MS_MSG_DELAY_END));   
    }//if

    // Interrupt mode pin values
    if(plgDigitalReadCfg.scan_mode == -1){ 
      if(newP0 != oldP0){ // P0 interrupt
        _process_pin(newP0, 3, oldP0);
        Serial.print(F(", 1 (P0+"));
        Serial.print(millis() - timeP0);
        Serial.println(FF(MS_MSG_DELAY_END));
        timeP0 = millis();
      }//if
      if(newP1 != oldP1){ // P1 interrupt
        Serial.print("0, ");
        _process_pin(newP1, 8, oldP1);
        Serial.print(F(" (P1+"));
        Serial.print(millis() - timeP1);
        Serial.println(FF(MS_MSG_DELAY_END));
      }//if 
      continue;    
    }//if
    
    // Scan Mode pin values
    _process_pin(digitalRead(INPUT_PIN), 3, oldP0);
    Serial.print(", ");
    _process_pin(digitalRead(INPUT_PULLUP_PIN), 8, oldP1);
    Serial.println();
    delay(cur_delay);
  }//while
}//plgDigitalRead
