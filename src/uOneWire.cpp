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

#include "uOneWire.h"

// Общие команды шины
#define SEARCH_ROM       0xF0 //Поиск устройств
#define DO_READ          0xFF //Если надо читать, то передаем 0xFF в exchange_byte

//** Сброс шины и ожидание ответа от устройства (1 - ok, 0 - все плохо)
uint8_t uOW_reset(const uint8_t pin){
  //Выставляем низкий уровень
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  delayMicroseconds(480); //Держим низкий уровень 480 микросекунд 

  //Ждем ответного импульса от устройства
  pinMode(pin, INPUT); //Переводим пин в режим чтения
  delayMicroseconds(70); //Ждем 70 микросекунд

  //Читаем ответный импульс
  uint8_t res = !digitalRead(pin);
  delayMicroseconds(410); //Ждем оставшиеся 410 микросекунд, чтобы устройство очухалось (480 = 410 + 70)
  return res; //Возвращаем результат (если 1 - все нормуль, если 0 - плохо)
}//uOW_reset


//** Производит обмен битом (младшим!) данных (для чтения передаем 1 и смотрим результат (если передать 0 - чтения не будет!), для записи на результат забиваем)
uint8_t _exchange_bit(const uint8_t pin, uint8_t data){
  pinMode(pin, OUTPUT);
  noInterrupts();
  digitalWrite(pin, LOW); //Выставляем низкий уровень (начинаем таймслот) 
  delayMicroseconds(3);

  if(data & 0x01){ // Передача единицы и чтения бита
    pinMode(pin, INPUT); // Отпустили шину (Резистор сам утащил ее в HIGH, считай передали 1)
    delayMicroseconds(10);// Ждем, чтобы устаканилось
    data = digitalRead(pin); // Читаем, что там прислали
  }//if
  else 
    delayMicroseconds(10); // Передача нуля. Чтение не производим. Ничего не делаем. Просто выравниваем время по сравнению с чтением/записью 1

  delayMicroseconds(67); // Ждем до окончания таймслота (таймслот 65: 13 на операции + 52 ожидания) + 15 пауза между таймслотами. т.е 52+15=67
  digitalWrite(pin, HIGH); // Возвращаем шину в высокий уровень
  interrupts();
  return data;  
}//_exchange_bit

  
//** Производит обмен байтом данных (для чтения передаем 0xFF и смотрим результат, для записи на результат забиваем)
/*
 * Все сурово:
 * 1. Отправляем на обмен младший бит данных
 * 2. Получаем младший бит из обмена
 * 3. Сдвигаем данные вправо
 * 4. Если пришедший бит = 1, ставим его старшим, иначе старшим будет 0
 * 5. Таким образом в цикле данные для записи постепенно заменяются данными чтения
*/
uint8_t uOW_exchangeByte(const uint8_t pin, uint8_t data){
  for(uint8_t i = 0; i < 8; i++) data = (data >> 1) | (_exchange_bit(pin, data) << 7);
  return data;  
}//uOW_exchangeByte


//** Чтение блока данных
void uOW_readBuf(const uint8_t pin, uint8_t * data, const uint8_t size){
  for(uint8_t i = 0; i < size; i++) data[i] = uOW_exchangeByte(pin, DO_READ);
}//uOW_readBuf


//** Вычисление CRC
uint8_t uOW_crc(uint8_t * addr, uint8_t len){
  uint8_t crc = 0;   

  while(len--){
    uint8_t inbyte = *addr++;
    for(uint8_t i = 8; i; i--){
      uint8_t mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if(mix) crc ^= 0x8c;
      inbyte >>= 1;
    }//for
  }//while
  return crc;
}//uOW_crc


//** Читает ROM в указанный буфер (с поиском!). Возвращает 1 - если все OK, 0 - если была ошибка чтения (тогда в errCode будет код ошибки)
uint8_t uOW_readROM(const uint8_t pin, uOW_ROM &rom, uOW_Errors & errCode)
{
  uint8_t * rom_ptr = (uint8_t *)&rom; // Будем работать со структурой линейно побайтно
  errCode = NO_DEVICE; 
  
  if(!uOW_reset(pin)) return 0; // Сброс шины не удался. Никто на сброс не ответил
  
  uOW_exchangeByte(pin, SEARCH_ROM);//Даем команду поиска ROM
  
  for(uint8_t rom_byte = 0; rom_byte < 8; rom_byte ++){ // Читаем все 8 байт
    uint8_t cur_byte = 0; // Текущий байт
    for(uint8_t rom_bit = 0; rom_bit < 8; rom_bit++) { // Читаем биты в байте
      uint8_t b1 = _exchange_bit(pin, 0x01); // Читаем первый бит
      uint8_t b2 = _exchange_bit(pin, 0x01); // Читаем второй (инверсный) бит

      if(!(b1 ^ b2)){ // Оба бита равны 1 или 0, значит работать дальше нельзя
        if(b1) errCode = NO_DEVICE; // Биты равны 1 - устройств нет
        else errCode = MULTIPLE_DEVICES; // Биты равны 0 - на шине несколько устройств
        return 0;
      }//if

      cur_byte |= (b1 << rom_bit); // Сохраняем текущий бит
      _exchange_bit(pin, b1); // Выдаем команду на чтение следующего бита
    }//for bit
    rom_ptr[rom_byte] = cur_byte; // Сохранили очередной байт
  }//for byte

  // CRC
  if(rom.crc != uOW_crc(rom_ptr, sizeof(rom) - 1)){
    errCode = CRC_ERROR;
    return 0;
  }//if
  
  errCode = NO_ERROR;
  return 1;
}


//** Выводит ведущий ноль для hex значений
void uOW_hex2dig(Print &p, uint8_t hex){
  if(hex < 0x10) p.print(0);
  p.print(hex, HEX);
}//uOW__hex2dig


//** Выводит в поток печати серийный номер в формате xx:xx:xx:xx:xx:xx
/*
void uOW_printSerial(Print &p, uint8_t ser[6]){
  for(uint8_t i = 0; i < 6; i++){
    uOW_hex2dig(p, ser[i]); // печать с ведущим нулем
    if(i < 5) p.print(":");
  }//for
}//uOW_printSerial
*/

//** Выводит в поток печати сообщение об ошибке
void uOW_printError(Print &p, const uint8_t errCode, const uint8_t wrong_val){
  switch(errCode){
    case NO_DEVICE: p.print(F("No device found")); break;
    case MULTIPLE_DEVICES: p.print(F("Multiple devices")); break;
    case CRC_ERROR: p.print(F("CRC error: 0x")); uOW_hex2dig(p, wrong_val); break;
    case WRONG_FAMILY_CODE: p.print(F("Wrong FCode:0x")); uOW_hex2dig(p, wrong_val); break;
    default: break;
  }//switch        
}//uOW_printError
