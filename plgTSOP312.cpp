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

#include "plgTSOP312.h"


#define INPUT_PIN P0

// Тайминги протокола NEC
#define NEC_ZERO_US         1125  // Время между спадами фронта для кодирования логического нуля
#define NEC_ONE_US          2250  // Время между спадами фронта для кодирования логической единицы
#define NEC_TIME_DELTA_US   150   // Разброс времени для кодирования логических 0/1
#define NEC_START_US        14560 // Стартовая последовательность 9000 high + 4500 low + 560 high 
#define NEC_START_DELTA_US  1500  // Разброс времени для стартовой последовательности

#define NEC_PKT_SIZE        32    // Размер пакета в битах


namespace TSOP312 {
  const char MSG_PRESS_KEY[] PROGMEM = "Press key on RC";                                    
  
  // Принимаемый пакет совмещенный с uin32_t для сборки бит за битом
  typedef union {
    uint32_t bin = 0;
    struct { // Структура принимаемого пакета
      uint8_t addr;     // Адрес
      uint8_t addr_inv; // Инверсный адрес
      uint8_t cmd;      // Команда
      uint8_t cmd_inv;  // Инверсная команда
    };    
  } pktBinary;

  volatile pktBinary pkt; // Принятый битовый пакет

  // Обработчик прерывания по спаду фронта сигнала
  void _inp_isr(){
    static pktBinary buf; // Буфер для сбора бит
    static uint32_t t_start = 0; // Время предыдущего вызова обработчика
    static uint8_t bit_cnt = 0; // Количество распознанных бит
    int8_t cur_bit = 2; // Текущиф распознанный бит (=2 пока ничего не распознано)
    uint32_t time = micros() - t_start; // Время между спадами фронта
    
    t_start += time;

    switch(time) {
      case (NEC_ZERO_US - NEC_TIME_DELTA_US) ... (NEC_ZERO_US + NEC_TIME_DELTA_US): // Это 0
        cur_bit = 0;
      break; 
      
      case (NEC_ONE_US - NEC_TIME_DELTA_US) ... (NEC_ONE_US + NEC_TIME_DELTA_US): // Это 1
        cur_bit = 1;
      break; 

      case (NEC_START_US - NEC_START_DELTA_US) ... (NEC_START_US + NEC_START_DELTA_US): // Это стартовая последовательность
        cur_bit = 2;
        buf.bin = 0;
        bit_cnt = 0;
      break; 

      default: // повторы нажатий не обслуживаем
      break;
    }//switch
    
    if(cur_bit > 1) return; // Интервал не соответствует ни одному из битов (это был повтор или стартовая последовательность)
    
    buf.bin = buf.bin | ((uint32_t)cur_bit << bit_cnt); // Собираем принятые биты
    bit_cnt ++; 
    cur_bit = 2;

    if(bit_cnt < NEC_PKT_SIZE) return; // Еще не набрали битов до полного пакета
    if(buf.addr != (uint8_t)~buf.addr_inv) return; // Инверсная часть адреса не соответствует прямой
    if(buf.cmd != (uint8_t)~buf.cmd_inv) return; // Инверсная часть команды не соответствует прямой

    // Все хорошо. Принят корректный пакет.
    pkt = buf; // Сложили принятые биты в пакет
    bit_cnt = 0; // Сбросили счетчик принятых бит
  }//_inp_isr
} //namespace


using namespace TSOP312;

// == Main plugin function ==
void plgTSOP312(){
  // Init
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN), _inp_isr, FALLING); // Ставим обработчик прерывания
  
  // Load settings from EEPROM 
    
  // Display Init
  core.moveCursor(0, 1);
  core.println(FF(MSG_PRESS_KEY));
  Serial.println(FF(MSG_PRESS_KEY));
 
  // Main loop
  while(1){
    if(!pkt.bin) continue; // Пока пакет нулевой -> ничего не принято
    
    core.moveCursor(0, 1); // Выводим на экран
    core.print(F("A: 0x"));
    core.print(core.rAlign(pkt.addr, 2, '0', HEX));
    core.print(F("  C: 0x"));
    core.println(core.rAlign(pkt.cmd, 2, '0', HEX));

    Serial.print(F("Addr: 0x")); // Выводим в Serial
    Serial.print(core.rAlign(pkt.addr, 2, '0', HEX));
    Serial.print(F(" Cmd: 0x"));
    Serial.println(core.rAlign(pkt.cmd, 2, '0', HEX));
    
    pkt.bin = 0; // Обнуляем пакет
  }//while
}//plgTSOP312
