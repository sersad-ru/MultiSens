#pragma once
#include <Arduino.h>

/*
* Micro OneWire Library
* 
* (c)2023 by sersad.ru
* 
* 25.11.2023
* 
* Сокращенная библиотека для работы по протоколу OneWire.
* Рассчитана на использование толькл ОДНОГО устройства на шине.
* Отсутствуют полные функции поиска и адресации. Только обмен данными с ЕДИНСТВЕННЫМ устройством.
* 
*/

// Коды ошибок
typedef enum {
  NO_ERROR,          // Ошибок не было
  NO_DEVICE,         // Устройств на шине не обнаружено
  MULTIPLE_DEVICES,  // Обнаружено более одного устройства на шине
  CRC_ERROR,         // CRC не сошлось
  WRONG_FAMILY_CODE, // Не верный код семейства устройств  
} uOW_Errors;


// Структура ROM
typedef struct {
 uint8_t familyCode; //Код типа устройства
 uint8_t serialNumber[6]; // Серийный номер
 uint8_t crc; //CRC
} uOW_ROM;


//** Сброс шины и ожидание ответа от устройства (1 - ok, 0 - все плохо)
uint8_t uOW_reset(const uint8_t pin);

//** Производит обмен байтом данных (для чтения передаем 0xFF и смотрим результат, для записи на результат забиваем)
uint8_t uOW_exchangeByte(const uint8_t pin, uint8_t data);

//** Чтение блока данных
void uOW_readBuf(const uint8_t pin, uint8_t * data, const uint8_t size);

//** Считает CRC. addr - указатель на буфер, len - размер буфера
uint8_t uOW_crc(uint8_t * addr, uint8_t len);

//** Читает ROM в указанный буфер (с поиском). Возвращает 1 - если все OK, 0 - если была ошибка чтения (тогда в errCode будет код ошибки)
uint8_t uOW_readROM(const uint8_t pin, uOW_ROM &rom, uOW_Errors &errCode);

//** Выводит в поток печати серийный номер в формате xx:xx:xx:xx:xx:xx
void uOW_printSerial(Print &p, uint8_t ser[6]);

//** Выводит в поток печати сообщение об ошибке
void uOW_printError(Print &p, const uint8_t errCode, const uint8_t wrong_val = 0);

//** Выводит ведущий ноль для hex значений
void uOW_hex2dig(Print &p, uint8_t hex);
