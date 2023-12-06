#include "plgWS2812.h"

#define DATA_PIN P0 // PD2


namespace WS2812 {

#define PORT portOutputRegister(digitalPinToPort(DATA_PIN))

void _write_buf(const uint8_t buf[WS_LED_CNT * 3]){
  uint8_t oldSREG;

  // Отключаетм прерывания
  oldSREG = SREG;
  cli();

  uint8_t set_high = (_BV(DATA_PIN) | *PORT);
  uint8_t set_low = (~_BV(DATA_PIN) & *PORT);

  //Погнали фигачить байты (состояние порта со сброшенным пином, потом состояние порта с установленным пином)
  for(uint8_t i = 0; i < WS_LED_CNT * 3; i++){
    asm volatile ( // Шлем один байт
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
      :[DATA]    "r" (buf[i]),
      [SET_HIGH] "r" (set_high),
      [SET_LOW]  "r" (set_low),
      "x" (PORT)
      :"r19", "r20"
    );//asm  
  }//for 

  // Включаем прерывания
  SREG = oldSREG;   

  digitalWrite(DATA_PIN, LOW); // Даем сброс
  delayMicroseconds(50);
}//_write_buf


void _update_item(int8_t &cur_led, const uint8_t item, const int8_t delta){
  switch(item){
    case 0: cur_led += delta; cur_led = max(cur_led, 0); cur_led = min(cur_led, WS_LED_CNT - 1); break;
    case 1: plgWS2812Cfg.leds[cur_led].r += delta; break;
    case 2: plgWS2812Cfg.leds[cur_led].g += delta; break;     
    case 3: plgWS2812Cfg.leds[cur_led].b += delta; break;     
    default: break;
  }//switch
}//_update_item


void _print_info(const int8_t cur_led){
  core.moveCursor(0, 1);
  core.print(cur_led);
  core.print(F(" R:"));
  core.print(core.rAlign(plgWS2812Cfg.leds[cur_led].r, 3));
  core.print(F(" G:"));
  core.print(core.rAlign(plgWS2812Cfg.leds[cur_led].g, 3));
  core.print(F(" B:"));
  core.print(core.rAlign(plgWS2812Cfg.leds[cur_led].b, 3));
}//_print_info  
}//namespase


using namespace WS2812;

// == Main plugin function ==
void plgWS2812() {
  //Init  
  // Load settings from EEPROM 
  if(!core.loadSettings((uint8_t*)&plgWS2812Cfg)){
    plgWS2812Cfg.leds[0] = {0, 16, 0};// Settings was reseted. Use default values 
    plgWS2812Cfg.leds[1] = {0, 0, 16};//GRB
    core.saveSettings((uint8_t*)&plgWS2812Cfg);// Save default value  
  }//if  

  pinMode(DATA_PIN, OUTPUT);

  _write_buf((uint8_t*) plgWS2812Cfg.leds);

  const uint8_t offsets[] = {0, 2, 8, 14};
  uint8_t item = 0; // Screen input item
  int8_t cur_led = 0;
  _print_info(cur_led);
  core.setCursorType(BLOCK);
  core.moveCursor(offsets[item], 1);   
  
  
  //Main loop
  while (1) {    
    // Process user input    
    switch (core.getButton()) {
      case UP:
      case UP_LONG: // Increase value of current item
        _update_item(cur_led, item, 1);
        _print_info(cur_led);
        core.moveCursor(offsets[item], 1);
        _write_buf((uint8_t*) plgWS2812Cfg.leds);
      break;
      
      case DOWN:
      case DOWN_LONG:  // Decrease value of current item
        _update_item(cur_led, item, -1);
        _print_info(cur_led);
        core.moveCursor(offsets[item], 1);
        _write_buf((uint8_t*) plgWS2812Cfg.leds);
      break;
      
      case SELECT: // Change current channel
        item++;
        if(item > 3) item = 0;
        core.moveCursor(offsets[item], 1);
      break;  
      
      case SELECT_LONG: core.saveSettings((uint8_t*)&plgWS2812Cfg); break;   // Save settings to EEPROM
      
      default: break;
    }//switch           
  }//while
}//plgWS2812
