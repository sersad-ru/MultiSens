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

#include "uBME280.h"
#include "mscore.h"
#include <Wire.h>


#define uBME280_BMP_ID 0x58 // BMP280 ID 
#define uBME280_BME_ID 0x60 // BME280 ID

#define uBME280_RESET_REG         0xE0 // Reset register
#define uBME280_RESET_VAL         0xB6 // Reset value

#define uBME280_ID_REG            0xD0 // ID register
#define uBME280_CALIBRATION_REG_1 0x88 // First Calibration register (BMP + BME) 25 bytes
#define uBME280_CALIBRATION_REG_2 0xE1 // Second Calibration register (BME only) 8 bytes
#define uBME280_HUM_CTRL_REG      0xF2 // Humidity cpntrol register (BME only)
#define uBME280_STATUS_REG        0xF3 // Status register
#define uBME280_CONTROL_REG       0xF4 // Control register
#define uBME280_CONFIG_REG        0xF5 // Config register
#define uBME280_TEMPERATURE_REG   0xFA // Temperature register (3 bytes) 
#define uBME280_PRESSURE_REG      0xF7 // Pressure register (3 bytes) 
#define uBME280_HUMIDITY_REG      0xFD // Humidity register (2 bytes)

// Oversampling
#define uBME280_OVERSAMPLING_1  0x01
#define uBME280_OVERSAMPLING_2  0x02
#define uBME280_OVERSAMPLING_4  0x03
#define uBME280_OVERSAMPLING_8  0x04
#define uBME280_OVERSAMPLING_16 0x05

// Operation modes
#define uBME280_MODE_SLEEP      0x00 // Sleep mode
#define uBME280_MODE_FORCED     0x01 // Forced mode
#define uBME280_MODE_NORMAL     0x03 // Normal mode

// Standby
#define uBME280_STANDBY_MS_05   0x00 // 50us
#define uBME280_STANDBY_MS_10   0x06 // 10ms
#define uBME280_STANDBY_MS_20   0x07 // 20ms
#define uBME280_STANDBY_MS_62_5 0x01 // 62.5ms
#define uBME280_STANDBY_MS_125  0x02 // 125ms
#define uBME280_STANDBY_MS_250  0x03 // 250ms
#define uBME280_STANDBY_MS_500  0x04 // 500ms
#define uBME280_STANDBY_MS_1000 0x05 // 1000ms

// Filter
#define uBME280_FILTER_OFF  0x00
#define uBME280_FILTER_2    0x01
#define uBME280_FILTER_4    0x02
#define uBME280_FILTER_8    0x03
#define uBME280_FILTER_16   0x04

//** Инициализация датчика. Данные для рассчета значений помещаются в структуру. Возвращает 1 - если прошло нормально, 0 - если устройство на нашлось
uint8_t uBME280_Init(const uint8_t i2c_addr, uBME280_Config &cfg){
  // Читаем ID
  cfg.id = core.i2cReadReg(i2c_addr, uBME280_ID_REG);  
  if((cfg.id != uBME280_BMP_ID) && (cfg.id != uBME280_BME_ID)){ // Illegal ID
    cfg.id = 0; 
    return 0;
  }//if
   
  // Сбрасываем
  core.i2cWriteReg(i2c_addr, uBME280_RESET_REG, uBME280_RESET_VAL);
  delay(10);

  // Дальше читаем калибровку 
  core.i2cRequestRead(i2c_addr, uBME280_CALIBRATION_REG_1, 25);
  cfg.t1 = (Wire.read() | (Wire.read() << 8));
  cfg.t2 = (Wire.read() | (Wire.read() << 8));
  cfg.t3 = (Wire.read() | (Wire.read() << 8));
  cfg.p1 = (Wire.read() | (Wire.read() << 8));
  cfg.p2 = (Wire.read() | (Wire.read() << 8));
  cfg.p3 = (Wire.read() | (Wire.read() << 8));
  cfg.p4 = (Wire.read() | (Wire.read() << 8));
  cfg.p5 = (Wire.read() | (Wire.read() << 8));
  cfg.p6 = (Wire.read() | (Wire.read() << 8));
  cfg.p7 = (Wire.read() | (Wire.read() << 8));
  cfg.p8 = (Wire.read() | (Wire.read() << 8));
  cfg.p9 = (Wire.read() | (Wire.read() << 8));
  cfg.h1 = Wire.read();

  core.i2cRequestRead(i2c_addr, uBME280_CALIBRATION_REG_2, 8);
  cfg.h2 = (Wire.read() | (Wire.read() << 8));
  cfg.h3 = Wire.read();
  cfg.h4 = (Wire.read() << 4);
  uint8_t tmp = Wire.read();
  cfg.h4 |= (tmp & 0x0F);
  cfg.h5 = (((tmp & 0xF0) >> 4) | (Wire.read() << 4));
  cfg.h6 = Wire.read();
    
  // Пишем настройки
  core.i2cWriteReg(i2c_addr, uBME280_CONTROL_REG, uBME280_MODE_SLEEP); // Отправляем в сон, иначе не воспримет натройки  
  core.i2cWriteReg(i2c_addr, uBME280_HUM_CTRL_REG, uBME280_OVERSAMPLING_1); // Humidity oversampling (for BME)
  core.i2cWriteReg(i2c_addr, uBME280_CONFIG_REG, (uBME280_STANDBY_MS_250 << 5) | (uBME280_FILTER_16 << 2)); // Standby and filter
  // Control последним, иначе настройки влажности не зайдут
  core.i2cWriteReg(i2c_addr, uBME280_CONTROL_REG, (uBME280_OVERSAMPLING_4 << 5) | (uBME280_OVERSAMPLING_2 << 2) | (uBME280_MODE_NORMAL)); // Oversampling Temperature, Pressure and Opeartion Mode    
  delay(300); // Ждем пока устаканится    
  return 1;
}//uBME280_Init


//** Внутренняя функция получения необработанной темеаратуры
int32_t _uBME280_get_temp_int(const uint8_t i2c_addr, const uBME280_Config cfg){
  int32_t raw = core.i2cReadReg(i2c_addr, uBME280_TEMPERATURE_REG, SIZE_24);

  if(raw == 0x800000) return 0;
  raw >>= 4;
  int32_t part1 = (((raw >> 3) - ((int32_t)cfg.t1 << 1)) * (int32_t)cfg.t2) >> 11;
  int32_t part2 = (((((raw >> 4) - (int32_t)cfg.t1) * ((raw >> 4) - (int32_t)cfg.t1)) >> 12) * ((int32_t)cfg.t3)) >> 14;
  return part1 + part2;  
}//_uBME280_get_temp_int


//** Возвращает температуру в градусах цельсия, умноженную на 1024. Т.е 27,54°С вернет как 2754  
int16_t uBME280_GetTemperature(const uint8_t i2c_addr, const uBME280_Config cfg){
  return (int32_t)((_uBME280_get_temp_int(i2c_addr, cfg) * 5 + 128) >> 8);
}//uBME280_GetTemperature


//** Возвращает давление в паскалях. Для перевода в мм рт. ст. делим на 133.
uint32_t uBME280_GetPressure(const uint8_t i2c_addr, const uBME280_Config cfg){
  uint32_t raw = core.i2cReadReg(i2c_addr, uBME280_PRESSURE_REG, SIZE_24);
  
  if(raw == 0x800000) return 0;
    
  raw >>= 4;
  int64_t part1 = (int64_t)_uBME280_get_temp_int(i2c_addr, cfg) - 128000;

  int64_t part2 = part1 * part1 * (int64_t)cfg.p6; 
  part2 = part2 + ((part1 * (int64_t)cfg.p5) << 17);
  part2 = part2 + ((int64_t)cfg.p4 << 35);
  part1 = ((part1 * part1 * (int64_t)cfg.p3) >> 8) + ((part1 * (int64_t)cfg.p2) << 12);
  part1 = ((((int64_t)1) << 47) + part1) * ((int64_t)cfg.p1) >> 33;

  if(!part1) return 0;

  int64_t res = 1048576 - raw;
  res = (((res << 31) - part2) * 3125) / part1;
  part1 = (((int64_t)cfg.p9) * (res >> 13) * (res >> 13)) >> 25;
  part2 = (((int64_t)cfg.p8) * res) >> 19;
  res = ((res + part1 + part2) >> 8) + (((int64_t)cfg.p7) << 4);

  return (uint32_t)(res >> 8);
}//uBME280_GetPressure


//** Возвращает влажность в %, умноженную на 100. Т.е. 67,75% вернет как 6775.  !!!ВНИМАНИЕ!!! На реальном устройстве не отлаживалась! Для BMP280 не используется!  
uint16_t uBME280_GetHumidity(const uint8_t i2c_addr, const uBME280_Config cfg){
  uint32_t raw = core.i2cReadReg(i2c_addr, uBME280_HUMIDITY_REG, SIZE_16);
   
  if(raw == 0x8000) return 0;

  int32_t part1 = _uBME280_get_temp_int(i2c_addr, cfg) - (int32_t)76800;  

  //!!!ВНИМАНИЕ!!! На реальном устройстве не отлаживалась!
  int32_t part2 = (
                   (raw << 14) - 
                   ((int32_t)cfg.h4 << 20) - 
                   ((int32_t)cfg.h5 * part1) + 
                   (int32_t)16384
                  ) >> 15;
                    
  int32_t part3 = (
                   ((part1 * (int32_t)cfg.h6) >> 10) * 
                   (((part1 * (int32_t)cfg.h3) >> 11) + (int32_t)32768)
                  ) >> 10;
                      
  part3 = ((part3 + (int32_t)2097152) * (int32_t)cfg.h2 + 8192) >> 14;

  part1 = part2 * part3;
  part1 = part1 - (((((part1 >> 15) * (part1 >> 15)) >> 7) * (int32_t)cfg.h1) >> 4);

  part1 = constrain(part1, 0, 419430400);
  return (int16_t)(((part1 >> 12) * 100) >> 10); // По даташиту part1 >> 12 - Это влажность * 1024. Остальное, что б получить влажность, умноженную на 100 
}//uBME280_GetHumidity
