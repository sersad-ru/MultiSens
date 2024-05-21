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

/*
* Сделать нормальный интерфейс. Вывод счетчика и направления вращения. Сброс счетчика в 0 по нажатию кнопки. 
* Вывод на экран и в порт.
*/
  // Main loop
  while(1){
    if(btn){ // Кнопка была нажата
      btn = 0; // Тут еще вывод обнулившегося счетчика
      counter = 0;
      Serial.print("BTN: ");      
      Serial.println(counter);
    }//if
    if(!rot) continue; // Если 0 - вращения не было
    counter += rot; 
    Serial.print((rot > 0)? "CW:  " : "CCW: ");
    Serial.println(counter);    
    rot = 0;
  }//while
}//plgREncoder
