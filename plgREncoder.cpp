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

#include "plgREncoder.h"


#define KEY_PIN P0
#define S1_PIN  P1
#define S2_PIN  P2

#define KEY_DEBOUNCE_MS 500 // Чувствительность нажатия кнопки в мс

namespace REncoder {

 int8_t counter = 0; // Просто счетчик для наглядности 
 volatile int8_t rot = 0; // Направление вращения. 0 - не вращается, 1 - по часовой, -1 - против часовой
 volatile uint8_t btn = 0; // 0 - не нажата, 1 - нажата

 
 // Обработчик прерывания поворота энкодера
 void _enc_isr(){
  static uint8_t last_s1 = 0; // Предыдущее состояние S1
  static uint8_t flag = 0; // У нас два импульса на один шаг, кроме первого
  uint8_t s1 = digitalRead(S1_PIN); // Читаем S1
  if(s1 == last_s1) return; // Первый выход не менялся, значит вращения не было
  flag = !flag; // Кроме первого нас интересует каждый второй импульс
  if(flag)rot = (digitalRead(S2_PIN) == last_s1) ? -1 : 1; // S2 == last_s1, т.е S2 != S1 - значит против часовой, иначе по часовой
  last_s1 = s1; // Сохранили текущее значени S1
 }// _enc_isr 


 // Обработчик прерывания нажатия на кнопку
 void _key_isr(){
  static uint32_t dt = 0;
  if((millis() - dt) < KEY_DEBOUNCE_MS) return;
  dt = millis();
  btn = !digitalRead(KEY_PIN);  
 }// _key_isr
} //namespace


using namespace REncoder;

// == Main plugin function ==
void plgREncoder(){
  // Init
  pinMode(S1_PIN, INPUT);
  pinMode(S2_PIN, INPUT);  
  pinMode(KEY_PIN, INPUT);  
  attachInterrupt(digitalPinToInterrupt(KEY_PIN), _key_isr, CHANGE); // Ставим обработчик прерывания кнопки
  attachInterrupt(digitalPinToInterrupt(S1_PIN), _enc_isr, CHANGE); // Ставим обработчик прерывания энкодера
 
  // Load settings from EEPROM 
    
  // Display Init
  core.moveCursor(0, 1);
  core.println(F("<NONE> Val: 0"));

  // Main loop
  while(1){
    if(btn){ // Кнопка была нажата
      btn = 0; 
      counter = 0;
      core.moveCursor(0, 1);
      core.print(F("<BTN>  Val: "));
      core.println(counter);
      
      Serial.print(F("<BTN>  Val: "));      
      Serial.println(counter);
    }//if
    
    if(!rot) continue; // Если 0 - вращения не было
    counter += rot; 
    core.moveCursor(0, 1);
    core.print((rot > 0)? F("<CW> ") : F("<CCW>"));
    core.print(F("  Val: "));
    core.println(counter);
    
    Serial.print((rot > 0)? F("<CW> ") : F("<CCW>"));
    Serial.print(F("  Val: "));
    Serial.println(counter);    
    rot = 0;
  }//while
}//plgREncoder
