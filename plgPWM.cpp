#include "plgPWM.h"


#define CH1_PIN P1
#define CH2_PIN P2
#define CH3_PIN P3

#define CH1_NUM 0
#define CH2_NUM 1
#define CH3_NUM 2

#define DEFAULT_DUTY 0

// const uint8_t (&offsets)[]
void _set_pwm(uint8_t pin_num, const uint8_t offsets[], const uint8_t pins[]){
  // Print new value
  core.moveCursor(offsets[pin_num], 1);
  core.print(core.rAlign(plgPWMCfg.duty[pin_num], 3));
  core.moveCursor(offsets[pin_num] - 3, 1);
  Serial.print(core.getPinName(pins[pin_num]));
  Serial.println(plgPWMCfg.duty[pin_num]);
  
  if(pin_num == CH3_NUM) return; // CH3 - soft PWM
  analogWrite(pins[pin_num], plgPWMCfg.duty[pin_num]); //set new value
}//_set_pwm


// == Main plugin function ==
void plgPWM(){
  // Init
  pinMode(CH3_PIN, OUTPUT);
  
  // Load settings from EEPROM 
  if(!core.loadSettings((uint8_t*)&plgPWMCfg)){
    plgPWMCfg.duty[CH1_NUM] = DEFAULT_DUTY;// Settings was reseted. Use default values
    plgPWMCfg.duty[CH2_NUM] = DEFAULT_DUTY;
    plgPWMCfg.duty[CH3_NUM] = DEFAULT_DUTY;
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

  // Cursor offsets and pins array 
  const uint8_t offsets[] = {3, 10, 17};
  const uint8_t pins[] = {CH1_PIN, CH2_PIN, CH3_PIN};

  uint8_t cur_channel = 0;
  uint8_t wrk_duty = plgPWMCfg.duty[CH3_NUM];
  uint8_t cnt = 0;    

  // Start with current duties
  for(uint8_t i = 0; i < 3; i++) _set_pwm(i, offsets, pins); // Start pwm
  core.moveCursor(offsets[cur_channel] - 3, 1); // Return cursor to current channel position
  
  // Main loop
  while(1){
    // Process user input    
    switch (core.getButton()) {
      case UP:
      case UP_LONG: // Increase duty of current channel
        plgPWMCfg.duty[cur_channel] ++;
        _set_pwm(cur_channel, offsets, pins);
      break;
      
      case DOWN:
      case DOWN_LONG:  // Decrease duty of current channel
        plgPWMCfg.duty[cur_channel] --;
        _set_pwm(cur_channel, offsets, pins);
      break;
      
      case SELECT: // Change current channel
        cur_channel++;
        if(cur_channel > arraySize(pins) - 1) cur_channel = 0;
        core.moveCursor(offsets[cur_channel] - 3, 1);
      break;  
      
      case SELECT_LONG: core.saveSettings((uint8_t*)&plgPWMCfg); break;   // Save settings to EEPROM
      
      default: break;
    }//switch       

   // software PWM
   if(!cnt){
     wrk_duty = plgPWMCfg.duty[CH3_NUM];
     if(wrk_duty) digitalWrite(CH3_PIN, HIGH);     
   }//if
   if((cnt == wrk_duty) && (wrk_duty < 255)) digitalWrite(CH3_PIN, LOW);
   cnt++;   
  }//while
}//plgPWM
