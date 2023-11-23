#pragma once
#include <Arduino.h>

/*
* Micro OneWire Library
* 
* Термометр DS18B20
* Таблетка DS1990A
*/

// Структура ROM
typedef struct {
 uint8_t familyCode; //Код типа устройства
 uint8_t serialNumber[6]; // Серийный номер
 uint8_t crc; //CRC
} uOW_ROM;


//** Сброс шины и ожидание ответа от устройства (1 - ok, 0 - все плохо)
uint8_t uOW_reset(uint8_t pin);

//** Производит обмен байтом данных (для чтения передаем 0xFF и смотрим результат, для записи на результат забиваем)
uint8_t uOW_exchangeByte(uint8_t pin, uint8_t data);

//** Чтение блока данных
void uOW_readBuf(uint8_t pin, uint8_t * data, uint8_t size);

//** Считает CRC. addr - указатель на буфер, len - размер буфера
uint8_t uOW_crc(uint8_t * addr, uint8_t len);

//** Читает ROM в указанный буфер. Возвращает 1 - если все OK, 0 - если была ошибка чтения
uint8_t uOW_readROM(uint8_t pin, uOW_ROM &rom);
