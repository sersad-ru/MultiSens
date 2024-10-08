/*
Arduino Multi Senstor Tester
(c)2023-2024 by Sergey Sadovnikov (sersad@gmail.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <www.gnu.org/licenses/>.
*/

#include "plgServo.h"


#define PWM_PIN P0

#define DEFAULT_ANGLE 0
#define MIN_PULSE_US 500
#define MAX_PULSE_US 2400
#define CYCLE_DELAY_MS 20
#define FULL_PATH_MS 300 // 0.3 c на поворот от 0 до 180 

namespace Servo {

 void _servo_pulse(const uint16_t pulse){
  cli();
  digitalWrite(PWM_PIN, HIGH);
  delayMicroseconds(pulse);
  digitalWrite(PWM_PIN, LOW);
  sei();
  delay(CYCLE_DELAY_MS - pulse / 1000);  // Ждем остатки до 20 ms 
 }//_servo_pulse

 
 void _set_angle(const uint8_t ang){
  int us = map(ang, 0, 180, MIN_PULSE_US, MAX_PULSE_US); // Считаем длительность импульса
  uint32_t start_time = millis();
  while((millis() - start_time) < FULL_PATH_MS) _servo_pulse(us); // Отдаем импульсы пока не совершится полные полоборота
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
