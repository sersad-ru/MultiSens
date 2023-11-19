#include "pAnalogRead.h"

#define WORK_PIN_1 P5
#define WORK_PIN_2 P6

#define DEFAULT_SCAN_MODE 4 //Use 500ms delay as default 

const uint16_t delays[] PROGMEM = {10, 50, 100, 250, 500, 1000, 1500, 2000}; // Variants of delays


void _process_pin(uint16_t new_val, uint8_t offset, uint16_t &oldVal){
  //to serial
  Serial.print(new_val);
  
  if(new_val == oldVal) return; // No changes

  //to display  
  core.moveCursor(offset, 1);
  core.print(core.rAlign(new_val, 4));

  //save time
  oldVal = new_val;
}//_process_pin


// == Main plugin function ==
void pAnalogRead(MultiSensCore& core){
  // Init

  // Load settings from EEPROM 
  if(!core.loadSettings((uint8_t*)&pAnalogReadCfg)){
    pAnalogReadCfg.scan_mode = DEFAULT_SCAN_MODE;// Settings was reseted. Use default values
    core.saveSettings((uint8_t*)&pAnalogReadCfg);// Save default value  
  }//if  

  uint16_t cur_delay = pgm_read_word(&delays[pAnalogReadCfg.scan_mode]);

  // Dispaly init
  core.moveCursor(0, 1); // First symbol of second line
  core.print(core.getPinName(WORK_PIN_1));
  core.moveCursor(8, 1);
  core.print(core.getPinName(WORK_PIN_2));
  core.moveCursor(16, 1);
  core.print(F("D: "));

  uint16_t oldPin1 = 2;
  uint16_t oldPin2 = 2;
  int8_t old_mode = -1;
    
  // Main loop
  while(1){
    // Process user input
    MultiSensButton btn = core.getButton();
    switch (btn) {
      case UP:
      case UP_LONG: pAnalogReadCfg.scan_mode++; break;
      
      case DOWN:
      case DOWN_LONG: pAnalogReadCfg.scan_mode--; break;
        
      case SELECT_LONG: core.saveSettings((uint8_t*)&pAnalogReadCfg);break;   // save settings to EEPROM
      
      default: break;
    }//switch

    if(pAnalogReadCfg.scan_mode < 0) pAnalogReadCfg.scan_mode = 0;
    if(pAnalogReadCfg.scan_mode > (int8_t)arraySize(delays) - 1) pAnalogReadCfg.scan_mode = arraySize(delays) - 1;

    // scan mode was changed?
    if(old_mode != pAnalogReadCfg.scan_mode){
      //Scan mode was changed
      old_mode = pAnalogReadCfg.scan_mode;
      core.moveCursor(18, 1);
      // delay mode
      cur_delay = pgm_read_word(&delays[pAnalogReadCfg.scan_mode]);
      core.println(cur_delay);
      Serial.print(F("P5, P6 ("));
      Serial.print(cur_delay);
      Serial.println(F("ms)"));   
    }//if

    // Scan Mode pin values
    _process_pin(analogRead(WORK_PIN_1), 3, oldPin1);
    Serial.print(", ");
    _process_pin(analogRead(WORK_PIN_2), 11, oldPin2);
    Serial.println();
    delay(cur_delay);
  }//while  
}//pAnalogRead
