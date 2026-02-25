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

#include "uAHT20.h"
#include "mscore.h"
#include <Wire.h>


#define uAHT20_CMD_RESET  0xBA // Reset command
#define uAHT20_CMD_INIT   0xBE // Init command
#define uAHT20_CMD_MEAS   0xAC // Start measurement
#define uAHT20_CMD_CALIB  0xE1 // Calibrate command

#define uAHT20_INIT_ARG 0x0800 // Init command arguments
#define uAHT20_MEAS_ARG 0x3300 // Measure command arguments

#define uAHT20_REG_STATUS     0x71 // Status Register (returns 0xFF if no device)
#define uAHT20_BUSY_MASK      0x80 // Busy bit 7
#define uAHT20_CALIB_MASK     0x08 // Calibrated bit 3 
#define uAHT20_NO_DEVICE      0xFF // No device found

//** Внутренняя функция ожидания с таймаутом. Ждет пока устройство освободится и выдает 1. Если не освободилось в течении таймаута - отдает 0
uint8_t _uAHT20_is_ready(const uint8_t i2c_addr){
  uint8_t cnt = 10; // Сколько циклов ждать
  while(core.i2cReadReg(i2c_addr, uAHT20_REG_STATUS) & uAHT20_BUSY_MASK){
    delay(10);
    if(!(cnt--)) return 0;
  }//while
  return 1;
}//_uAHT20_is_ready


//** Инициализация датчика. Текущий статус помещается в структуру. Возвращает 1 - если прошло нормально, 0 - если устройство на нашлось
uint8_t uAHT20_Init(const uint8_t i2c_addr, uAHT20_Data &data){
  // Ждем 40 мс что б датчик ожил. На случай, если все только что включилось
  delay(40);
  // Читаем статус
  data.status = core.i2cReadReg(i2c_addr, uAHT20_REG_STATUS);
  if(data.status == uAHT20_NO_DEVICE) return 0; // не нашли устройство

  // Проверяем откалиброван ли
  if(!(data.status & uAHT20_CALIB_MASK)){ // Не откалиброван
    // Инициализируем
    core.i2cWriteReg(i2c_addr, uAHT20_CMD_INIT, uAHT20_INIT_ARG, SIZE_16);
    if(!_uAHT20_is_ready(i2c_addr)) return 0; // Устройство так и не стало готово после долгого ожидания
    if(!(core.i2cReadReg(i2c_addr, uAHT20_REG_STATUS) & uAHT20_CALIB_MASK)){ data.status = uAHT20_NO_DEVICE; return 0;} // не откалибровался
  }//if
  
  return 1;
}//uAHT20_Init


//** Получение данных с датчика. Результаты измерений помещаются в структуру. Если получить данные неудалось, возвращает 0. Иначе 1
uint8_t uAHT20_Read(const uint8_t i2c_addr, uAHT20_Data &data){
 // Запускаем измерение
 core.i2cWriteReg(i2c_addr, uAHT20_CMD_MEAS, uAHT20_MEAS_ARG, SIZE_16);
 delay(75); //Ждем
 if(!_uAHT20_is_ready(i2c_addr)) return 0; // Устройство так и не стало готово после долгого ожидания
    
 //Читаем 6 байт данных
 core.i2cRequestRead(i2c_addr, uAHT20_REG_STATUS, 6);
 Wire.read(); // Skip first byte (status)

 uint32_t tmp = ((uint32_t)Wire.read() << 16) | ((uint32_t)Wire.read() << 8);
 uint8_t h_t = Wire.read();
 data.hum = (((tmp | (uint32_t)h_t) >> 4) * 10000) >> 20; // Влажность в процентах * 100

 data.temp = (((int64_t)((((uint32_t)h_t & 0x0F) << 16) | ((uint32_t)Wire.read() << 8) | (uint32_t)Wire.read()) * 20000) >> 20) - 5000; // Температура * 100 
  
 return 1;
}//uAHT20_Read
