#include "plgServo.h"


#define PWM_PIN P0

#define DEFAULT_ANGLE 0
#define MIN_PULSE_US 500
#define MAX_PULSE_US 2400
#define CYCLE_DELAY_MS 20
#define PULSE_COUNT 10 // Сколько раз повторять импульсы, чтобы точно хватило развернуться на 180 радусов

namespace Servo {

 void _servo_pulse(const uint16_t pulse){
  cli();
  bitWrite(PORTD, 2, 1);
  delayMicroseconds(pulse);
  bitWrite(PORTD, 2, 0);
  sei();
  delay(CYCLE_DELAY_MS - pulse / 1000);  // Ждем остатки до 20 ms 
 }//_servo_pulse

 
 void _set_angle(const uint8_t ang){
  int us = map(ang, 0, 180, MIN_PULSE_US, MAX_PULSE_US); // Считаем длительность импульса
  for(uint8_t i = 0; i < PULSE_COUNT; i++) _servo_pulse(us);
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
  core.print(F("Angle:   "));
  core.print(MS_SYM_DEGREE_CODE);
  core.print(" ");
  core.print(MS_SYM_SELECT_CODE);
  core.println(F("-Go!"));

  // Set servo to position from EEPROM
  _set_angle(plgServoCfg.angle);
  
    // Main loop
  while(1){
    // Display current position
    core.moveCursor(6, 1);
    core.print(core.rAlign(plgServoCfg.angle, 3));            
    
    // Process user input    
    switch (core.wait4Button()) {
      case UP:
      case UP_LONG: // Increase angle of servo
        plgServoCfg.angle += 5;
        if(plgServoCfg.angle > 180) plgServoCfg.angle = 0;
      break;
      
      case DOWN:
      case DOWN_LONG:  // Decrease angle of servo
        plgServoCfg.angle -= 5;
        if(plgServoCfg.angle > 180) plgServoCfg.angle = 180; // т.к. беззнаковое, то ловим переход через 0 таким обрзом
      break;
      
      case SELECT: // Rotate the servo drive
        _set_angle(plgServoCfg.angle);      
        Serial.print(plgServoCfg.angle);
        Serial.println("°");
      break;  
      
      case SELECT_LONG: core.saveSettings((uint8_t*)&plgServoCfg); break;   // Save settings to EEPROM
      
      default: break;
    }//switch          
  }//while
}//plgServo
