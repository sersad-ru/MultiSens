#include "plgREncoder.h"


#define S1_PIN  P0
#define S2_PIN  P1
#define KEY_PIN P2


namespace REncoder {
  
 // Обработчик прерывания поворота энкодера
 void _enc_isr(){
 }// _enc_isr 
 
} //namespace


using namespace REncoder;

// == Main plugin function ==
void plgREncoder(){
  // Init
  pinMode(S1_PIN, INPUT);
  pinMode(S2_PIN, INPUT);  
  pinMode(KEY_PIN, INPUT);  
  //attachInterrupt(digitalPinToInterrupt(KEY_PIN), _key_isr, CHANGE); // Ставим обработчик прерывания кнопки
  attachInterrupt(digitalPinToInterrupt(S1_PIN), _enc_isr, CHANGE); // Ставим обработчик прерывания энкодера
 
  // Load settings from EEPROM 
    
  // Display Init
 
  // Main loop
  while(1){
  }//while
}//plgREncoder
