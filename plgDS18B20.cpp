#include "plgDS18B20.h"
#include "uOneWire.h"

#define WORK_PIN P0

/*
 * Точность pad.conf
* #define SSDS18B20_09BIT 0x1F  1/2 от raw
* #define SSDS18B20_10BIT 0x3F  1/4
* #define SSDS18B20_11BIT 0x5F  1/8
* #define SSDS18B20_12BIT 0x7F  1/16

*//** Код семейства
*#define SSDS18B20_CODE 0x28

*/

// Структура Scratchpad
typedef struct {
 uint8_t tempLSB; // Младший байт температуры
 uint8_t tempMSB; // Старший байт температуры
 uint8_t tH; // Регистр тревоги верхнего предела
 uint8_t tL; // Регистр тревоги нижнего предела
 uint8_t conf; // Регистр конфигурации
 uint8_t res1; // Зарезервировано (FF)
 uint8_t res2; // Зарезервировано (0C)
 uint8_t res3; // Зарезервировано (10)
 uint8_t crc; // CRC
} uOW_Scratchpad;


// Команды DS18B20 
#define SKIP_ROM         0xCC //Пропустить выбор устройства по адресу (обратиться ко всем устройствам сразу)
#define READ_SCRATCHPAD  0xBE //Прочесть данные
#define WRITE_SCRATCHPAD 0x4E //Записать данные в ОЗУ
#define CONVERT          0x44 //Получить температуру
#define COPY_SCRATCHPAD  0x48 //Скопировать данные из ОЗУ в ПЗУ




//** Читает Scratchpad в заданный буфер.  Возвращает 0 - если была ошибка, 1 - если все ок
uint8_t uOW_readScratchpad(uint8_t pin, uOW_Scratchpad &data){
  if(!uOW_reset(pin)) return 0; // Сброс шины не удался
  
  uOW_exchangeByte(pin, SKIP_ROM);//Выдаем команду SKIP_ROM, т.к. устройство всего одно
  uOW_exchangeByte(pin, READ_SCRATCHPAD); //Выдаем команду чтения скратчпада
  uOW_readBuf(pin, (uint8_t*)&data, sizeof(data)); // Читаем данный в буфер

  return data.crc == uOW_crc((uint8_t*)&data, sizeof(data) - 1); // CRC сошлось или нет
}//uOW_readScratchpad  


//** Отдает команду получения температуры. Ждет 800ms пока дело сделается. Потом надо читать scratchpad.
//   Возвращает 0 - если была ошибка, 1 - если все ок
uint8_t uOW_prepareTemp(uint8_t pin){
  if(!uOW_reset(pin)) return 0; // Сброс шины не удался
  
  uOW_exchangeByte(pin, SKIP_ROM);//Выдаем команду SKIP_ROM, т.к. устройство всего одно
  uOW_exchangeByte(pin, CONVERT); //Выдаем команду получения температуры

  digitalWrite(pin, HIGH); //Переводим шину в высокое состояние, чтобы у датчика была энергия на получение температуры
  delay(800); //Ждем 800 милисекунд до окончания получения температуры

  return 1;
}//uOW_prepareTemp


//** Отдает команду копирования скратчпада в ПЗУ
uint8_t uOW_copyScratchpad(uint8_t pin){
  if(!uOW_reset(pin)) return 0; // Сброс шины не удался
  
  uOW_exchangeByte(pin, SKIP_ROM);//Выдаем команду SKIP_ROM, т.к. устройство всего одно
  uOW_exchangeByte(pin, COPY_SCRATCHPAD); //Выдаем команду копирования скратчпада

  digitalWrite(pin, HIGH); //Переводим шину в высокое состояние, чтобы у датчика была энергия на получение температуры
  delay(15); //Ждем 15 милисекунд до окончания команды
  
  uOW_reset(pin); // Заканчиваем объмен 
  return 1; 
}//uOW_copyScratchpad



// == Main plugin function ==
void plgDS18B20(){
  // Init 
  uOW_ROM rom;
  uOW_Scratchpad pad;
   
  Serial.print(uOW_readROM(WORK_PIN, rom));
  Serial.print(": ");
  Serial.println(rom.familyCode, HEX);
    
  for(byte i = 0; i < sizeof(rom.serialNumber); i++){
    Serial.print(rom.serialNumber[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  Serial.println(uOW_prepareTemp(WORK_PIN));
  
  Serial.print(uOW_readScratchpad(WORK_PIN, pad));
  Serial.print(": ");
  Serial.println(pad.conf, HEX);
  
  int32_t raw = (pad.tempMSB << 8) | pad.tempLSB;  
  Serial.println(raw);
  //Serial.println((int32_t)(raw * 100));
  Serial.print((int32_t)((raw) / 16));
  Serial.print(".");
  Serial.println((int32_t)((raw * 100) % 16));
  //Serial.println((float)raw * 0.0625);
  
  
  // Main loop
  while(1){
  }//while
}//
