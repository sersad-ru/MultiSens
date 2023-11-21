#include "plgFindPin.h"

const char pFP_NOT_FOUND[] PROGMEM = "Not found";

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
  core.print(FF(pFP_NOT_FOUND));       
  Serial.println(FF(pFP_NOT_FOUND));
}//_findpin


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
