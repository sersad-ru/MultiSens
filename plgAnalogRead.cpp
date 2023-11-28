#include "plgAnalogRead.h"

#define WORK_PIN_1 P5
#define WORK_PIN_2 P6

#define DEFAULT_SCAN_MODE 4 //Use 500ms delay as default. See MS_STD_DELAYS in mscore.h


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
void plgAnalogRead(){
  // Init

  // Load settings from EEPROM 
  if(!core.loadSettings((uint8_t*)&plgAnalogReadCfg)){
    plgAnalogReadCfg.scan_mode = DEFAULT_SCAN_MODE;// Settings was reseted. Use default values
    core.saveSettings((uint8_t*)&plgAnalogReadCfg);// Save default value  
  }//if  

  uint16_t cur_delay = pgm_read_word(&MS_STD_DELAYS[plgAnalogReadCfg.scan_mode]);

  // Dispaly init
  core.moveCursor(0, 1); // First symbol of second line
  core.print(core.getPinName(WORK_PIN_1));
  core.moveCursor(8, 1);
  core.print(core.getPinName(WORK_PIN_2));
  core.moveCursor(16, 1);
  core.print(FF(MS_MSG_DELAY_NAME));

  uint16_t oldPin1 = 2;
  uint16_t oldPin2 = 2;
  int8_t old_mode = -1;
    
  // Main loop
  while(1){
    // Process user input    
    switch (core.getButton()) {
      case UP:
      case UP_LONG: plgAnalogReadCfg.scan_mode++; break;
      
      case DOWN:
      case DOWN_LONG: plgAnalogReadCfg.scan_mode--; break;
        
      case SELECT_LONG: core.saveSettings((uint8_t*)&plgAnalogReadCfg);break;   // save settings to EEPROM
      
      default: break;
    }//switch

    plgAnalogReadCfg.scan_mode = max(plgAnalogReadCfg.scan_mode, 0);
    plgAnalogReadCfg.scan_mode = min(plgAnalogReadCfg.scan_mode, (int8_t)arraySize(MS_STD_DELAYS) - 1);


    // was scan mode changed?
    if(old_mode != plgAnalogReadCfg.scan_mode){
      //Scan mode was changed
      old_mode = plgAnalogReadCfg.scan_mode;
      core.moveCursor(18, 1);
      // delay mode
      cur_delay = pgm_read_word(&MS_STD_DELAYS[plgAnalogReadCfg.scan_mode]);
      core.println(cur_delay);
      Serial.print(F("P5, P6 ("));
      Serial.print(cur_delay);
      Serial.println(FF(MS_MSG_DELAY_END));   
    }//if

    // Scan Mode pin values
    _process_pin(analogRead(WORK_PIN_1), 3, oldPin1);
    Serial.print(", ");
    _process_pin(analogRead(WORK_PIN_2), 11, oldPin2);
    Serial.println();
    delay(cur_delay);
  }//while  
}//plgAnalogRead
