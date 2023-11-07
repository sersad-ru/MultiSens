#include "pFindPin.h"

void pFindPin(MultiSensCore& core){
  // Title        
  core.moveCursor(4, 0);
  core.println(F("Find Pin"));
  core.print(F("pin->GND,"));
  core.print(MS_SYM_SELECT_CODE);
  core.print(F("-start."));

  while(1){
    delay(1000);
  }
  
}//pFindPin
