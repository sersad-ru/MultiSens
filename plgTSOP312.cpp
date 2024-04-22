#include "plgTSOP312.h"


#define INPUT_PIN P0


// Тайминги протокола NEC
#define NEC_ZERO_US       1125  // Время между спадами фронта для кодирования логического нуля
#define NEC_ONE_US        2250  // Время между спадами фронта для кодирования логической единицы
#define NEC_TIME_DELTA_US  150  // Разброс времени для кодирования логических 0/1

namespace TSOP312 {

  // Переменные, обрабатываемые в прерывании
  volatile uint32_t buf = 0; // Буфер приема последовательности импульсов
  void _inp_isr(){
    static uint32_t t_start = 0; // Время предыдущего вызова обработчика
    static uint8_t bit_cnt = 0; // Количество распознанных бит
    int8_t cur_bit = 2; // Текущиф распознанный бит (пока ничего не распознано)
    uint32_t time = micros() - t_start; // Время между спадами фронта
    t_start += time;

    /*
    * Тут сделать нормальное распознование протокола !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    */
    
    if((time > (NEC_ZERO_US - NEC_TIME_DELTA_US)) && (time < (NEC_ZERO_US + NEC_TIME_DELTA_US))) cur_bit = 0;
    if((time > (NEC_ONE_US - NEC_TIME_DELTA_US)) && (time < (NEC_ONE_US + NEC_TIME_DELTA_US))) cur_bit = 1;
    
    if(cur_bit > 1) return; // Пока тут просто выход
   
    buf = (buf << 1) | cur_bit;
    cur_bit = 2;
    bit_cnt ++; 
    if(bit_cnt >= 32){
     Serial.println(buf, HEX);
     bit_cnt = 0;
    } 
  }//_inp_isr
} //namespace


using namespace TSOP312;

// == Main plugin function ==
void plgTSOP312(){
  // Init
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN), _inp_isr, FALLING);
  // Load settings from EEPROM 
    
  // Display Init
 
  // Main loop
  while(1){
  }//while
}//plgTSOP312
