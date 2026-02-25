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

/*
* Micro AHT20 Library
* 
* 14.01.2024
* 
* Минимальные функции для получения данных с датчика AHT20
* 
*/

// AHT20 Данные
typedef struct {
  uint8_t status; // device status = uAHT20_NO_DEVICE (0xFF) если устройства не найдено
  uint16_t hum;   // влажность x 100
  int16_t temp;   // температура x 100  
} uAHT20_Data;

//** Инициализация датчика. Текущий статус помещается в структуру. Возвращает 1 - если прошло нормально, 0 - если устройство на нашлось
//* i2c_addr - адрес устройства на шине i2c
//* data - данные устройства (на этом этапе только статус)
uint8_t uAHT20_Init(const uint8_t i2c_addr, uAHT20_Data &data);


//** Получение данных с датчика. Результаты измерений помещаются в структуру. Если получить данные неудалось, возвращает 0. Иначе 1
//* i2c_addr - адрес устройства на шине i2c
//* data - данные устройства (температура х 100, влажность х 100 и последний статус)
uint8_t uAHT20_Read(const uint8_t i2c_addr, uAHT20_Data &data);
