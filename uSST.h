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

#pragma once
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>
/*
* Micro SoftwareSerial Tools Library
* 
* 20.06.2024
* 
* Минимальные функции для Обработки строк из COM-порта
* 
*/
/* Сообщения при поиске устройства на порту */
const char uSST_NO_DEV_MSG[]     PROGMEM = "No device found";  
const char uSST_CONNECTING_MSG[] PROGMEM = "Connecting...";  
const char uSST_FOUND_MSG[]      PROGMEM = "Found at ";  

// Прототип функции проверки соединения для поиска скорости ком-порта
//* Возвращает 0 - если соединение не установилось, 1 - если установилось
//* ser - Ссылка на Software serial
//* buf - буфер для приема строки
//* buf_size - размер буфера
//* timeout_ms - таймаут в ms
typedef uint8_t (*uSST_ProbeFunction)(const SoftwareSerial &ser, char* buf, const uint8_t buf_size, const uint32_t timeout_ms);


//** Читает строку из Serial в буфер до появления /n, /r или таймаута. Отрезает /n, /r. Ставит ноль в конце. 
//* Возвращает длину строки (включая завершающий ноль) или  0 - если таймаут
//* ser - Ссылка на Software serial
//* buf - буфер для приема строки
//* buf_size - размер буфера
//* timeout_ms - таймаут в ms
uint8_t uSST_ReadString(const SoftwareSerial &ser, char* buf, const uint8_t buf_size, const uint32_t timeout_ms);
uint8_t uSST_ReadString(const HardwareSerial &ser, char* buf, const uint8_t buf_size, const uint32_t timeout_ms);


//** Последовательно открывеат порт на разных скоростях (от большей к меньшей) и вызывает функцию проверки. 
//*  Если проверка прошла - возвращает скорость порта. Если ни одной проверка не прошло - возвращает 0.
//*  В первый раз открывает порт на скорости guess_speed (если она не нулевая), а потом уже идет по списку скоростей.
//* ser - Ссылка на Software serial
//* probe - ссылка на функцию проверки соединения (см. typedef выше)
//* buf - буфер для приема строки
//* buf_size - размер буфера
//* timeout_ms - таймаут в ms
//* guess_speed [0] - предполагаемая скорость. Сначала проверяем на ней, если нет, то тогда уже все по списку.
uint32_t uSST_FindSpeed(const SoftwareSerial &ser, uSST_ProbeFunction probe, char* buf, const uint8_t buf_size, const uint32_t timeout_ms, const uint32_t guess_speed = 0);


//** Ищет n-ое входжение символа в строку. Возвращает индекс или -1, если нужного входжения не найдено 
//* str - строка
//* sym - символ
//* n - номер входжения 
int8_t uSST_strchrn(char* str, const char sym, const uint8_t n);
