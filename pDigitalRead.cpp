#include "pDigitalRead.h"

//#define WORK_PIN P0
// == Main plugin function ==
void pDigitalRead(MultiSensCore& core){
  // Init
  core.print(F("P0->input "));
  core.print(MS_SYM_SELECT_CODE);
  core.print(F("-start"));
  pinMode(P0, INPUT);
  
/*
  pDigitalReadCfg.a = 25;
  core.saveSettings((uint8_t*)&pDigitalReadCfg);  

  bool res = core.loadSettings((uint8_t*)&pDigitalReadCfg);  
  Serial.println(res);
  Serial.println(pDigitalReadCfg.a);
*/  
  while(1){
    /*
     * Тут нормально написать на пару каналов. С регулировкой задержки..
     * Задержки по списку их пресета, скажем 100, 250, 500, 1000, 1500, 2000 ms
     * Для варианта меньше 100 - сделать по прерываниям на RISING (первый канал) и FALLING (второй канал)
    */
    core.moveCursor(0, 1); // First symbol of second line
    core.print(F("P0:"));
    core.println(digitalRead(P0));
    Serial.println(digitalRead(P0));
    delay(1000);
  }//while
}//pDigitalRead
