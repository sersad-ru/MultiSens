#include "pAnalogRead.h"

//#define WORK_PIN P5
// == Main plugin function ==
void pAnalogRead(MultiSensCore& core){
  // Init
  core.print(F("P5->input "));
  //core.print(MS_SYM_SELECT_CODE);
  //core.print(F("-start"));
  pinMode(P5, INPUT);
    
  // Main loop
  while(1){

    core.moveCursor(0, 1); // First symbol of second line
    core.print(F("P5:"));
    core.println(analogRead(P5));
    Serial.println(analogRead(P5));
    delay(1000);  
  }//while  
}//pAnalogRead
