#pragma once
#include <Arduino.h>

/*
* Micro BME280 Library
* 
* (c)2024 by sersad.ru
* 
* 14.01.2024
* 
* Минимальные функции для получения данных с датчика BME280 / BMP280
* 
*/

// BMP(E) config
  typedef struct {
    uint8_t  id = 0; // BMP/BME ID. Zero if no device present
    
    uint16_t t1 = 0;
    int16_t  t2 = 0;
    int16_t  t3 = 0;
    
    uint16_t p1 = 0;
    int16_t  p2 = 0;
    int16_t  p3 = 0;
    int16_t  p4 = 0;
    int16_t  p5 = 0;
    int16_t  p6 = 0;
    int16_t  p7 = 0;
    int16_t  p8 = 0;
    int16_t  p9 = 0;
    
    uint16_t h1 = 0; // For BME
    int16_t  h2 = 0;
    uint8_t  h3 = 0;
    int16_t  h4 = 0;
    int16_t  h5 = 0;
    int8_t   h6 = 0;
  } uBME280_Config;


//** Инициализация датчика. Данные для рассчета значений помещаются в структуру. Возвращает 1 - если прошло нормально, 0 - если устройство на нашлось
//* i2c_addr - адрес устройства на шине i2c
//* cfg - конфигурация устройства
uint8_t uBME280_Init(const uint8_t i2c_addr, uBME280_Config &cfg);

//** Возвращает температуру в градусах цельсия, умноженную на 1024. Т.е 27,54°С вернет как 2754  
//* i2c_addr - адрес устройства на шине i2c
//* cfg - конфигурация устройства
int16_t uBME280_GetTemperature(const uint8_t i2c_addr, const uBME280_Config cfg);

//** Возвращает давление в паскалях. Для перевода в мм рт. ст. делим на 133.
//* i2c_addr - адрес устройства на шине i2c
//* cfg - конфигурация устройства
uint32_t uBME280_GetPressure(const uint8_t i2c_addr, const uBME280_Config cfg);

//** Возвращает влажность в %, умноженную на 100. Т.е. 67,75% вернет как 6775.  !!!ВНИМАНИЕ!!! На реальном устройстве не отлаживалась!
//* i2c_addr - адрес устройства на шине i2c
//* cfg - конфигурация устройства
uint16_t uBME280_GetHumidity(const uint8_t i2c_addr, const uBME280_Config cfg);
