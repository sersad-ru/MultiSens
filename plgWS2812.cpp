#include "plgWS2812.h"

#define DATA_PIN P0 // PD2

#define LED_CNT 2 // LED count (2-3 max)

namespace WS2812 {

// WS2812 color space
typedef struct {
  uint8_t g;
  uint8_t r;
  uint8_t b;
} leds;

#define PORT portOutputRegister(digitalPinToPort(DATA_PIN))

void _write_byte(uint8_t data, const uint8_t set_low, const uint8_t set_high){
  // Отправка байта
  asm volatile (
    "ldi r19, 8           \n\t"  // 8 бит в байте
    "_MAIN_LOOP_%=:       \n\t"  // Начало цикла
    "st X, %[SET_HIGH]    \n\t"  // Высокий уровень на выходе
    "sbrs %[DATA], 7      \n\t"  // Пропуск следующего шага, если старший бит = 1
    "st X, %[SET_LOW]     \n\t"  // Низкий уровень на выходе
    "lsl %[DATA]          \n\t"  // Сдвигаем данные влево
    
    "ldi r20, 3           \n\t"  // 8 тактов, 2 цикла по 3 + загрузка 1 + nop
    "_DELAY_LOOP%=:       \n\t"  // Задержка циклом
    "dec r20              \n\t"  // декремент 1 такт
    "brne _DELAY_LOOP%=   \n\t"  // Конец цикла задержки (2 такта)
    "nop                  \n\t"  // nop (1 такт)             

    "st X, %[SET_LOW]     \n\t"  // Низкий уровень
    "dec r19              \n\t"  // Счетчик внешнего цикла
    "brne _MAIN_LOOP_%=   \n\t"  // Конец внешнего цикла
    :
    :[DATA]    "r" (data),
    [SET_HIGH] "r" (set_high),
    [SET_LOW]  "r" (set_low),
    "x" (PORT)
    :"r19", "r20"
  );//asm
}//write_byte


void _write_buf(const uint8_t buf[LED_CNT * 3]){
  uint8_t oldSREG;

  // Отключаетм прерывания
  oldSREG = SREG;
  cli();

  //Погнали фигачить байты (состояние порта со сброшенным пином, потом состояние порта с установленным пином)
  for(uint8_t i = 0; i < LED_CNT * 3; i++) _write_byte(buf[i], (~_BV(DATA_PIN) & *PORT), (_BV(DATA_PIN) | *PORT));

  // Включаем прерывания
  SREG = oldSREG;  
}//_write_buf


}//namespase

using namespace WS2812;

// == Main plugin function ==
void plgWS2812() {
  //Init  
  leds buf[LED_CNT] = {{16, 0, 0}, {0, 0, 16}}; //GRB

  pinMode(DATA_PIN, OUTPUT);

  _write_buf((uint8_t*) buf);
  
  //Main loop
  while (1) {
  }//while
}//plgWS2812
