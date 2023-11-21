#include "plgPWM.h"


#define CH1_PIN P1
#define CH2_PIN P2
#define CH3_PIN P3

#define CH1_NUM 0
#define CH2_NUM 1
#define CH3_NUM 2

#define DEFAULT_DUTY 0


void _set_pwm(uint8_t pin, uint8_t duty){
  if(pin == CH3_PIN) return; // CH3 - soft PWM
  analogWrite(pin, duty);
}//_set_pwm


// == Main plugin function ==
void plgPWM(MultiSensCore& core){
  // Init
  pinMode(CH3_PIN, OUTPUT);
  
  // Load settings from EEPROM 
  if(!core.loadSettings((uint8_t*)&plgPWMCfg)){
    plgPWMCfg.duty[CH1_NUM] = DEFAULT_DUTY;// Settings was reseted. Use default values
    plgPWMCfg.duty[CH2_NUM] = DEFAULT_DUTY;// Settings was reseted. Use default values
    plgPWMCfg.duty[CH3_NUM] = DEFAULT_DUTY;// Settings was reseted. Use default values    
    core.saveSettings((uint8_t*)&plgPWMCfg);// Save default value  
  }//if  

  // Display Init
  core.moveCursor(0, 1); // First symbol of second line
  core.print(core.getPinName(CH1_PIN));  
  core.moveCursor(7, 1); 
  core.print(core.getPinName(CH2_PIN));
  core.moveCursor(14, 1); 
  core.print(core.getPinName(CH3_PIN));
  core.setCursorType(BLOCK);

  //  
  const uint8_t offsets[] = {3, 10, 17};
  const uint8_t pins[] = {CH1_PIN, CH2_PIN, CH3_PIN};  
  uint8_t cur_channel = 0;
  uint8_t wrk_duty = plgPWMCfg.duty[CH3_NUM];
  uint8_t cnt = 0;    


  // Start with current duties
  for(uint8_t i = 0; i < 3; i++){
    core.moveCursor(offsets[i], 1);
    core.print(core.rAlign(plgPWMCfg.duty[i], 3));
    Serial.print(core.getPinName(pins[i]));
    Serial.println(plgPWMCfg.duty[i]);
    _set_pwm(pins[i], plgPWMCfg.duty[i]); // Start pwm
  }//for
  core.moveCursor(offsets[cur_channel] - 3, 1);
  
  // Main loop
  while(1){
    // Process user input    
    switch (core.getButton()) {
      case UP:
      case UP_LONG:  
        plgPWMCfg.duty[cur_channel] ++;
        core.moveCursor(offsets[cur_channel], 1);
        core.print(core.rAlign(plgPWMCfg.duty[cur_channel], 3));
        core.moveCursor(offsets[cur_channel] - 3, 1);
        Serial.print(core.getPinName(pins[cur_channel]));
        Serial.println(plgPWMCfg.duty[cur_channel]);        
        _set_pwm(pins[cur_channel], plgPWMCfg.duty[cur_channel]); // Change pwm
      break;
      
      case DOWN:
      case DOWN_LONG:  
        plgPWMCfg.duty[cur_channel] --;
        core.moveCursor(offsets[cur_channel], 1);
        core.print(core.rAlign(plgPWMCfg.duty[cur_channel], 3));
        core.moveCursor(offsets[cur_channel] - 3, 1);
        Serial.print(core.getPinName(pins[cur_channel]));
        Serial.println(plgPWMCfg.duty[cur_channel]);        
        _set_pwm(pins[cur_channel], plgPWMCfg.duty[cur_channel]); // Change pwm
      break;
      
      case SELECT: 
        cur_channel++;
        if(cur_channel > 2) cur_channel = 0;
        core.moveCursor(offsets[cur_channel] - 3, 1);
      break;  
      
      case SELECT_LONG: core.saveSettings((uint8_t*)&plgPWMCfg); break;   // save settings to EEPROM
      
      default: break;
    }//switch       

   // software PWM
   if(!cnt){
     wrk_duty = plgPWMCfg.duty[CH3_NUM];
     if(wrk_duty) digitalWrite(CH3_PIN, HIGH);     
   }//if
   if(cnt == wrk_duty) digitalWrite(CH3_PIN, LOW);
   cnt++;   
  }//while
}//plgPWM
