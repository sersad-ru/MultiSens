#include "pDigitalRead.h"

#define INPUT_PIN P0
#define INPUT_PULLUP_PIN P1

#define DEFAULT_SCAN_MODE 3 //Use 500ms delay as default 

const uint16_t delays[] PROGMEM = {0, 100, 250, 500, 1000, 1500, 2000}; // Variants of delays

    /*
     * Для варианта меньше 100 - сделать по прерываниям на RISING (первый канал) и FALLING (второй канал)
    */


void _process_pin(uint8_t pin, uint8_t offset, uint8_t &oldVal, uint32_t &oldTime){
  uint8_t new_val = digitalRead(pin);   

  Serial.print(core.getPinName(pin));
  Serial.print(new_val);
  
  if(new_val == oldVal){ // No changes
    Serial.println();
    return;
  }//if

  //to display  
  core.moveCursor(offset, 1);
  core.print(new_val);

  //to serial
  Serial.print(F(" (+"));
  Serial.print(millis() - oldTime);
  Serial.println(F(" ms)"));

  //save time
  oldVal = new_val;
  oldTime = millis();
}//_process_pin



// == Main plugin function ==
void pDigitalRead(MultiSensCore& core){
  // Init    
  pinMode(INPUT_PIN, INPUT);
  pinMode(INPUT_PULLUP_PIN, INPUT_PULLUP);

  // Load settings from EEPROM 
  if(!core.loadSettings((uint8_t*)&pDigitalReadCfg)){
    pDigitalReadCfg.scan_mode = DEFAULT_SCAN_MODE;// Settings was reseted. Use default values
    core.saveSettings((uint8_t*)&pDigitalReadCfg);// Save default value  
  }//if  

  uint16_t cur_delay = pgm_read_word(&delays[pDigitalReadCfg.scan_mode]);
  
  // Dispaly init
  core.moveCursor(0, 1); // First symbol of second line
  core.print(core.getPinName(INPUT_PIN));
  core.moveCursor(5, 1);
  core.print(core.getPinName(INPUT_PULLUP_PIN));
  core.moveCursor(10, 1);
  core.print(F("D: "));

  uint8_t oldP1 = 2;
  uint32_t timeP1 = millis();
  uint8_t oldP2 = 2;
  uint32_t timeP2 = millis();
  int8_t old_mode = -1;
  
  
  // Main loop
  while(1){
    /*
     * Для варианта меньше 100 - сделать по прерываниям на RISING (первый канал) и FALLING (второй канал)
    */
    MultiSensButton btn = core.getButton();
    switch (btn) {
      case UP:
      case UP_LONG: pDigitalReadCfg.scan_mode--; break;
      
      case DOWN:
      case DOWN_LONG: pDigitalReadCfg.scan_mode++; break;
        
      case SELECT_LONG: break;   // запись настроек
      
      default: break;
    }//switch

    if(pDigitalReadCfg.scan_mode < 0) pDigitalReadCfg.scan_mode = 0;
    if(pDigitalReadCfg.scan_mode > (int8_t)arraySize(delays) - 1) pDigitalReadCfg.scan_mode = arraySize(delays) - 1;

    if(old_mode != pDigitalReadCfg.scan_mode){
      //Scan mode was changed
      old_mode = pDigitalReadCfg.scan_mode;
      core.moveCursor(12, 1);
      if(pDigitalReadCfg.scan_mode == 0){ // Interrupt mode
        core.println(F("INT"));
        Serial.println(F("Interrup mode."));        
        continue;
      }//if     
      cur_delay = pgm_read_word(&delays[pDigitalReadCfg.scan_mode]);
      core.println(cur_delay);
      Serial.print(F("Delay: "));
      Serial.println(cur_delay);
    }//if

    if(pDigitalReadCfg.scan_mode != 0){ // Scan Mode
      //Process pins
      _process_pin(INPUT_PIN, 3, oldP1, timeP1);
      _process_pin(INPUT_PULLUP_PIN, 8, oldP2, timeP2);
    }//if  
    delay(cur_delay);
  }//while
}//pDigitalRead
