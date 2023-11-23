#include "uOneWire.h"

/*
 *  Тут еще всякие searc-и? 
 */

// Общие команды шины
#define READ_ROM         0x33 //Прочесть ПЗУ
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


//** Производит обмен битом данных (для чтения передаем 1 и смотрим результат, для записи на результат забиваем)
uint8_t _exchange_bit(const uint8_t pin, uint8_t data){
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW); //Выставляем низкий уровень (начинаем таймслот)
  delayMicroseconds(7); //Держим его половину интервала в 15 us

  if(data) digitalWrite(pin, HIGH); //Если нужно передать 1 - возвращаем шину на высокий уровень
  delayMicroseconds(8); //Ждем оставшиеся 8 микросекунд. При передаче 0 - получаем все 15 us низкого уровня

  //Теперь читаем
  data = digitalRead(pin);
  delayMicroseconds(45); //Ждем 45 микросекунд до окончания таймслота
  digitalWrite(pin, HIGH); //Возвращаем шину в высокий уровень
  
  delayMicroseconds(15); //Ждем 15 микросекунд для паузы между таймслотами 
  return data;
}//_exchange_bit


//** Производит обмен байтом данных (для чтения передаем 0xFF и смотрим результат, для записи на результат забиваем)
/*
 * Все сурово:
 * 1. Отправляем на обмен младший бит данных
 * 2. Получаем младший бит
 * 3. Сдвигаем данные вправо
 * 4. Если пришедший бит = 1, ставим его старшим, иначе старшим будет 0
 * 5. Таким образом в цикле данные для записи постепенно заменяются данными чтения
*/
uint8_t uOW_exchangeByte(const uint8_t pin, uint8_t data){
  for(uint8_t i = 0; i < 8; i++) data = _exchange_bit(pin, data & 0x01) ? (data >> 1) | (1 << 7) : data >> 1;
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


//** Читает ROM в заданный буфер. Возвращает 0 - если ошибка, 1 - если все ok
uint8_t uOW_readROM(const uint8_t pin, uOW_ROM &rom){
  if(!uOW_reset(pin)) return 0; // Сброс шины не удался

  uOW_exchangeByte(pin, READ_ROM);//Даем команду чтения ROM (у нас всегда только одно устройство на шине!)

  uOW_readBuf(pin, (uint8_t*)&rom, sizeof(rom)); // Читаем ROM
  uOW_reset(pin);// Дальше читать не надо. Сбрасываем шину

  return rom.crc == uOW_crc((uint8_t*)&rom, sizeof(rom) - 1); // CRC сошлось или нет
}//uOW_readROM
