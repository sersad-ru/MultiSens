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

#include "plgFindPin.h"

namespace FindPin {
  
const char plgFP_NOT_FOUND[] PROGMEM = "Not found";

void _find_pin(){
  uint8_t pins[] = {2, 3, 11, 12, 13, 17, 18, 19};
  for(uint8_t i = 0; i < arraySize(pins); i++){
    core.print(MS_SYM_PROGRESS_CODE);
    delay(250); 
    core.print(MS_SYM_PROGRESS_CODE);
    pinMode(pins[i], INPUT_PULLUP);
    if(!digitalRead(pins[i])){
      core.moveCursor(0, 1);
      core.print("P");
      core.print(i);
      Serial.print("P");
      Serial.println(i);
      return;
    }//if
  delay(250); 
  }//for
  
  core.moveCursor(0, 1); 
  core.print(FF(plgFP_NOT_FOUND));       
  Serial.println(FF(plgFP_NOT_FOUND));
}//_findpin

} //namespace

using namespace FindPin;
// == Main plugin function ==
void plgFindPin(){
  // Init
  core.moveCursor(0, 1); // First symbol of second line
  core.print(F("pin->GND,"));
  core.print(MS_SYM_SELECT_CODE);
  core.print(F("-start."));
    
  // Main loop
  while(1){
    if(core.wait4Button() != SELECT) continue; // Press <SELECT> to start scanning

    core.moveCursor(0, 1); // First symbol of second line
    core.println();   
    _find_pin();
    core.print(" | ");
    core.print(MS_SYM_SELECT_CODE);
    core.println(F("-next scan"));
  }//while  
}//plgFindPin
