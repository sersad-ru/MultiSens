#include "plgServo.h"


#define PWM_PIN P0

#define DEFAULT_ANGLE 0
#define MIN_PULSE_US 500
#define MAX_PULSE_US 2400
#define CYCLE_DELAY_MS 20

namespace Servo {

 void _set_angle(const uint8_t ang){
  int us = map(ang, 0, 180, MIN_PULSE_US, MAX_PULSE_US); // Convert degrees to the pulse length   
  
  static uint32_t oldms = millis();
  if((millis() - oldms) < CYCLE_DELAY_MS) return;  
//  int us = (ang * 11) + 500; // Convert degrees to the pulse length 
  //digitalWrite(PWM_PIN, HIGH);
  bitWrite(PORTD, 2, 1);
  delayMicroseconds(us);
  //digitalWrite(PWM_PIN, LOW); 
  bitWrite(PORTD, 2, 0);
 }//_set_angle

} //namespace


using namespace Servo;

// == Main plugin function ==
void plgServo(){
  // Init
  pinMode(PWM_PIN, OUTPUT);
  
  // Load settings from EEPROM 
  if(!core.loadSettings((uint8_t*)&plgServoCfg)){
    plgServoCfg.angle = DEFAULT_ANGLE;// Settings was reseted. Use default values
    core.saveSettings((uint8_t*)&plgServoCfg);// Save default value  
  }//if  

  // Display Init
  core.moveCursor(0, 1);
  core.print(F("Angle:  0"));
  core.print(MS_SYM_DEGREE_CODE);
  core.print(" ");
  core.print(MS_SYM_SELECT_CODE);
  core.println(F("-Go!"));
  uint8_t angle = plgServoCfg.angle;
  // Main loop
  while(1){
    // Process user input    
    switch (core.getButton()) {
      case UP:
      case UP_LONG: // Increase angle of servo
        plgServoCfg.angle ++;
        if(plgServoCfg.angle > 180) plgServoCfg.angle = 0;
        core.moveCursor(6, 1);
        core.print(core.rAlign(plgServoCfg.angle, 3));
      break;
      
      case DOWN:
      case DOWN_LONG:  // Decrease angle of servo
        plgServoCfg.angle --;
        if(plgServoCfg.angle > 254) plgServoCfg.angle = 180;
        core.moveCursor(6, 1);
        core.print(core.rAlign(plgServoCfg.angle, 3));        
      break;
      
      case SELECT: // Rotate the servo drive
        angle = plgServoCfg.angle;
        Serial.print(angle);
        Serial.println("°");
      break;  
      
      case SELECT_LONG: core.saveSettings((uint8_t*)&plgServoCfg); break;   // Save settings to EEPROM
      
      default: break;
    }//switch       
   _set_angle(angle);
  }//while
}//plgPWM
