#include "plgStepper.h"


#define IN1_PIN P0 // P0
#define IN2_PIN P1 // P1
#define IN3_PIN P2 // P2
#define IN4_PIN P3 // P3

#define STEPS 2048 // Steps per full revolution
#define STEPS_PER_SECOND 500 // 500 steps per second 

namespace Stepper {

  void _do_step(const uint8_t step_num){
    switch(step_num){
      case 0: // 1010 -> 1100
        digitalWrite(IN1_PIN, HIGH); 
        digitalWrite(IN2_PIN, HIGH);  
        digitalWrite(IN3_PIN, LOW); 
        digitalWrite(IN4_PIN, LOW);  
      break;
      case 1: // 0110 -> 0110 
        digitalWrite(IN1_PIN, LOW);
        digitalWrite(IN2_PIN, HIGH);
        digitalWrite(IN3_PIN, HIGH);
        digitalWrite(IN4_PIN, LOW);
      break;
      case 2: // 0101 -> 0011
        digitalWrite(IN1_PIN, LOW);
        digitalWrite(IN2_PIN, LOW);
        digitalWrite(IN3_PIN, HIGH);
        digitalWrite(IN4_PIN, HIGH);
      break;
      case 3: // 1001 -> 1001
        digitalWrite(IN1_PIN, HIGH);
        digitalWrite(IN2_PIN, LOW);
        digitalWrite(IN3_PIN, LOW);
        digitalWrite(IN4_PIN, HIGH);
      break;     
      default: break;
    }//switch
    delayMicroseconds(2000);  // 90 градусов в секунду. Тут затеять пересчет и изменение скорости (замедление)
  }//_do_step

  
  void _go(int16_t deg){
    uint16_t start = 0; // С какого шага начинать. Прямое направление. По часовой.
    uint16_t stop = abs(map(deg, 0, 360, 0, STEPS)); // На каком шаге заканчивать (с переводм из градусов в шаги)
    int8_t d = 1; // направление
    
    if(deg < 0){ // Обратное направление (против часовой)
      d = -1;
      start = stop;
      stop = 0;
    }//if
    
    for(int16_t i = start; i != stop; i+= d ) _do_step(i % 4);
  }//_step
} //namespace


using namespace Stepper;

/*
* Скорость в градусах в секунду
* Ну и пользовательский интерфейс
* И доку написать (картинки готовы)
*/

// == Main plugin function ==
void plgStepper(){
  // Init
  pinMode(P0, OUTPUT);
  pinMode(P1, OUTPUT);
  pinMode(P2, OUTPUT);
  pinMode(P3, OUTPUT);
    
  // Display Init
   
  _go(360);
  
  // Main loop
  while(1){
    
  }//while
}//plgStepper
