#include "plgDigitalRead.h"

#define INPUT_PIN P0
#define INPUT_PULLUP_PIN P1

#define DEFAULT_SCAN_MODE 5 //Use 500ms delay as default 

const uint16_t delays[] PROGMEM = {0, 10, 50, 100, 250, 500, 1000, 1500, 2000}; // Variants of delays

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

  uint16_t cur_delay = pgm_read_word(&delays[plgDigitalReadCfg.scan_mode]);
  
  // Dispaly init
  core.moveCursor(0, 1); // First symbol of second line
  core.print(core.getPinName(INPUT_PIN));
  core.moveCursor(5, 1);
  core.print(core.getPinName(INPUT_PULLUP_PIN));
  core.moveCursor(10, 1);
  core.print(F("D: "));

  uint8_t oldP0 = 2;
  uint8_t oldP1 = 2;
  uint32_t timeP0 = millis();
  uint32_t timeP1 = millis();
  int8_t old_mode = -1;
  
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

    plgDigitalReadCfg.scan_mode = max(plgDigitalReadCfg.scan_mode, 0);
    plgDigitalReadCfg.scan_mode = min(plgDigitalReadCfg.scan_mode, (int8_t)arraySize(delays) - 1);
    
    // scan mode was changed?
    if(old_mode != plgDigitalReadCfg.scan_mode){
      //Scan mode was changed
      old_mode = plgDigitalReadCfg.scan_mode;
      core.moveCursor(12, 1);
      if(plgDigitalReadCfg.scan_mode == 0){ // Interrupt mode
        core.println(F("INT"));
        Serial.println(F("P0, P1 (Interrup mode.)"));        
        attachInterrupt(digitalPinToInterrupt(INPUT_PIN), _p0_isr, CHANGE);
        attachInterrupt(digitalPinToInterrupt(INPUT_PULLUP_PIN), _p0_isr, CHANGE);
        continue;
      }//if     
      // delay mode
      detachInterrupt(digitalPinToInterrupt(INPUT_PIN));
      detachInterrupt(digitalPinToInterrupt(INPUT_PULLUP_PIN));
      cur_delay = pgm_read_word(&delays[plgDigitalReadCfg.scan_mode]);
      core.println(cur_delay);
      Serial.print(F("P0, P1 ("));
      Serial.print(cur_delay);
      Serial.println(F("ms)"));   
    }//if

    // Interrupt mode pin values
    if(plgDigitalReadCfg.scan_mode == 0){ 
      if(newP0 != oldP0){ // P0 interrupt
        _process_pin(newP0, 3, oldP0);
        Serial.print(F(", 1 (P0+"));
        Serial.print(millis() - timeP0);
        Serial.println(F("ms)"));
        timeP0 = millis();
      }//if
      if(newP1 != oldP1){ // P1 interrupt
        Serial.print("0, ");
        _process_pin(newP1, 8, oldP1);
        Serial.print(F(" (P1+"));
        Serial.print(millis() - timeP1);
        Serial.println(F("ms)"));
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
