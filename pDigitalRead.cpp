#include "pDigitalRead.h"

void pDigitalRead(MultiSensCore& core){
  // Title
  core.print(F("DigitalRead. P1->input. "));
  core.print(MS_SYM_SELECT_CODE);
  core.print(F(" - to start."));

  while(1){
    delay(1000);
  }

  //пусто
}//pDigitalRead