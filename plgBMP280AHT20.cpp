#include "plgBMP280AHT20.h"
#include <Wire.h>

#define SDA_PIN P6 //Yellow-Black
#define SCL_PIN P7 //Gray-Black
#define READ_DELAY_MS 500 // 0.5 seconds between attempt

#define BMP_ADDRESS 0x77 // BMP280 I2C address
#define AHT_ADDRESS 0x38 // AHT20 I2C address 

// -- BMP(E)--
#define BMP_ID 0x58 // BMP280 ID 
#define BME_ID 0x60 // BME280 ID

#define BMP_RESET_REG       0xE0 // Reset register
#define BMP_RESET_VAL       0xB6 // Reset value

#define BMP_ID_REG          0xD0 // ID register
#define BMP_CALIBRATION_REG 0x88 // First Calibration register (BMP + BME) 25 bytes
#define BME_CALIBRATION_REG 0xE1 // Second Calibration register (BME only) 8 bytes
#define BME_HUM_CTRL_REG    0xF2 // Humidity cpntrol register (BME only)
#define BMP_STATUS_REG      0xF3 // Status register
#define BMP_CONTROL_REG     0xF4 // Control register
#define BMP_CONFIG_REG      0xF5 // Config register
#define BMP_TEMPERATURE_REG 0xFA // Temperature register (3 bytes) 
#define BMP_PRESSURE_REG    0xF7 // Pressure register (3 bytes) 
#define BME_HUMIDITY_REG    0xFD // Humidity register (2 bytes)

// Oversampling
#define BMP_OVERSAMPLING_1  0x01
#define BMP_OVERSAMPLING_2  0x02
#define BMP_OVERSAMPLING_4  0x03
#define BMP_OVERSAMPLING_8  0x04
#define BMP_OVERSAMPLING_16 0x05

// Operation modes
#define BMP_MODE_SLEEP      0x00 // Sleep mode
#define BMP_MODE_FORCED     0x01 // Forced mode
#define BMP_MODE_NORMAL     0x03 // Normal mode

// Standby
#define BMP_STANDBY_MS_05   0x00 // 50us
#define BMP_STANDBY_MS_10   0x06 // 10ms
#define BMP_STANDBY_MS_20   0x07 // 20ms
#define BMP_STANDBY_MS_62_5 0x01 // 62.5ms
#define BMP_STANDBY_MS_125  0x02 // 125ms
#define BMP_STANDBY_MS_250  0x03 // 250ms
#define BMP_STANDBY_MS_500  0x04 // 500ms
#define BMP_STANDBY_MS_1000 0x05 // 1000ms

// Filter
#define BMP_FILTER_OFF  0x00
#define BMP_FILTER_2    0x01
#define BMP_FILTER_4    0x02
#define BMP_FILTER_8    0x03
#define BMP_FILTER_16   0x04

// -- AHT --
#define AHT_CMD_RESET  0xBA // Reset command
#define AHT_CMD_INIT   0xBE // Init command
#define AHT_CMD_MEAS   0xAC // Start measurement
#define AHT_CMD_CALIB  0xE1 // Calibrate command

#define AHT_INIT_ARG 0x0800 // Init command arguments
#define AHT_MEAS_ARG 0x3300 // Measure command arguments

#define AHT_REG_STATUS     0x71 // Status Register (returns 0xFF if no device)
#define AHT_BUSY_MASK      0x80 // Busy bit 7
#define AHT_CALIB_MASK     0x08 // Calibrated bit 3 
#define AHT_NO_DEVICE      0xFF // No device found

namespace BMP280AHT20 {
  // BMP(E) settings 
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
  } BMPSettings;


  void _bmp_init(BMPSettings &result){
    // Читаем ID
    result.id = core.i2cReadReg(BMP_ADDRESS, BMP_ID_REG);  
    if((result.id != BMP_ID) && (result.id != BME_ID)){ // Illegal ID
      result.id = 0; 
      return;
    }//if
   
    // Сбрасываем
    core.i2cWriteReg(BMP_ADDRESS, BMP_RESET_REG, BMP_RESET_VAL);
    delay(10);

    // Дальше читаем калибровку 
    core.i2cRequestRead(BMP_ADDRESS, BMP_CALIBRATION_REG, 25);
    result.t1 = (Wire.read() | (Wire.read() << 8));
    result.t2 = (Wire.read() | (Wire.read() << 8));
    result.t3 = (Wire.read() | (Wire.read() << 8));
    result.p1 = (Wire.read() | (Wire.read() << 8));
    result.p2 = (Wire.read() | (Wire.read() << 8));
    result.p3 = (Wire.read() | (Wire.read() << 8));
    result.p4 = (Wire.read() | (Wire.read() << 8));
    result.p5 = (Wire.read() | (Wire.read() << 8));
    result.p6 = (Wire.read() | (Wire.read() << 8));
    result.p7 = (Wire.read() | (Wire.read() << 8));
    result.p8 = (Wire.read() | (Wire.read() << 8));
    result.p9 = (Wire.read() | (Wire.read() << 8));
    result.h1 = Wire.read();

    core.i2cRequestRead(BMP_ADDRESS, BME_CALIBRATION_REG, 8);
    result.h2 = (Wire.read() | (Wire.read() << 8));
    result.h3 = Wire.read();
    result.h4 = (Wire.read() << 4);
    uint8_t tmp = Wire.read();
    result.h4 |= (tmp & 0x0F);
    result.h5 = (((tmp & 0xF0) >> 4) | (Wire.read() << 4));
    result.h6 = Wire.read();
    
    // Пишем настройки
    core.i2cWriteReg(BMP_ADDRESS, BMP_CONTROL_REG, BMP_MODE_SLEEP); // Отправляем в сон, иначе не воспримет натройки  
    core.i2cWriteReg(BMP_ADDRESS, BME_HUM_CTRL_REG, BMP_OVERSAMPLING_1); // Humidity oversampling (for BME)
    core.i2cWriteReg(BMP_ADDRESS, BMP_CONFIG_REG, (BMP_STANDBY_MS_250 << 5) | (BMP_FILTER_16 << 2)); // Standby and filter
    // Control последним, иначе настройки влажности не зайдут
    core.i2cWriteReg(BMP_ADDRESS, BMP_CONTROL_REG, (BMP_OVERSAMPLING_4 << 5) | (BMP_OVERSAMPLING_2 << 2) | (BMP_MODE_NORMAL)); // Oversampling Temperature, Pressure and Opeartion Mode    
    delay(300); // Ждем пока устаканится   
  }//_bmp_init


  int32_t _bmp_temp_int(const BMPSettings cfg){ // Температура, умноженная на 100
    int32_t raw = core.i2cReadReg(BMP_ADDRESS, BMP_TEMPERATURE_REG, SIZE_24);
    
    if(raw == 0x800000) return 0;
    raw >>= 4;
    int32_t part1 = (((raw >> 3) - ((int32_t)cfg.t1 << 1)) * (int32_t)cfg.t2) >> 11;
    int32_t part2 = (((((raw >> 4) - (int32_t)cfg.t1) * ((raw >> 4) - (int32_t)cfg.t1)) >> 12) * ((int32_t)cfg.t3)) >> 14;
    return part1 + part2;
  }//_bmp_temp


  int32_t _bmp_temp(const BMPSettings cfg){ // Температура, умноженная на 100
    return (_bmp_temp_int(cfg) * 5 + 128) >> 8;
  }//_bmp_temp


  uint32_t _bmp_pres(const BMPSettings cfg){ // Давление в паскалях.  Делим на 133 и получаем мм рт.ст.
    uint32_t raw = core.i2cReadReg(BMP_ADDRESS, BMP_PRESSURE_REG, SIZE_24);
      
    if(raw == 0x800000) return 0;
    
    raw >>= 4;
    int64_t part1 = (int64_t)_bmp_temp_int(cfg) - 128000;
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
  }//_bmp_pres


  uint32_t _bmp_hum(const BMPSettings cfg){ // Влажность * 1024. Для BMP280 не используется!  
    uint32_t raw = core.i2cReadReg(BMP_ADDRESS, BME_HUMIDITY_REG, SIZE_16);
   
    if(raw == 0x8000) return 0;

    int32_t part1 = _bmp_temp_int(cfg) - (int32_t)76800;
    
    // ATTENTION!!! Not tested on a real device!
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
    return part1 >> 12;
  }//_bmp_hum


// AHT settings 
  typedef struct {
    uint8_t status; // device status
    uint16_t hum;   // humidity x 100
    int16_t temp;   // temperature x 100
  } AHTData;

  void _aht_init(AHTData &result){
    // Читаем статус (тут еще может быть dalay(40), что б раскачалось, если сразу опрос)
    result.status = core.i2cReadReg(AHT_ADDRESS, AHT_REG_STATUS);
    if(result.status == AHT_NO_DEVICE) return; // не нашли устройство

    // Проверяем откалиброван ли
    if(!(result.status & AHT_CALIB_MASK)){ // Не откалиброван
      // Инициализируем
      core.i2cWriteReg(AHT_ADDRESS, AHT_CMD_INIT, AHT_INIT_ARG, SIZE_16);
      while(core.i2cReadReg(AHT_ADDRESS, AHT_REG_STATUS) & AHT_BUSY_MASK) delay(10); // Ждем пока не будет готов
      if(!(core.i2cReadReg(AHT_ADDRESS, AHT_REG_STATUS) & AHT_CALIB_MASK)){ result.status = AHT_NO_DEVICE; return;} // не откалибровался
    }//if
  }// _aht_init
  

 void _aht_read(AHTData &result){
   // Запускаем измерение
   core.i2cWriteReg(AHT_ADDRESS, AHT_CMD_MEAS, AHT_MEAS_ARG, SIZE_16);
   delay(75); //Ждем
   while(core.i2cReadReg(AHT_ADDRESS, AHT_REG_STATUS) & AHT_BUSY_MASK) delay(10); // Ждем вдруг занят
   
   //Читаем 6 байт данных
   core.i2cRequestRead(AHT_ADDRESS, AHT_REG_STATUS, 6);
   Wire.read(); // Skip first byte (status)

   uint32_t tmp = ((uint32_t)Wire.read() << 16) | ((uint32_t)Wire.read() << 8);
   uint8_t h_t = Wire.read();
   result.hum = (((tmp | (uint32_t)h_t) >> 4) * 10000) >> 20; // Влажность в процентах * 100

   result.temp = (((int64_t)((((uint32_t)h_t & 0x0F) << 16) | ((uint32_t)Wire.read() << 8) | (uint32_t)Wire.read()) * 20000) >> 20) - 5000; // Температура * 100 
 }// _aht_read
  
} //namespace

using namespace BMP280AHT20;


// == Main plugin function ==
void plgBMP280AHT20(){
  // Init
  Wire.begin();   

  AHTData aht;
  _aht_init(aht);
  Serial.println(aht.status, HEX);
  
  if(aht.status == AHT_NO_DEVICE) Serial.println(F("No AHT20 found"));

  BMPSettings bmp;
  _bmp_init(bmp);  

  if(!bmp.id) Serial.println(F("No BMP280 found"));

  // Serial Header
  if(aht.status != AHT_NO_DEVICE) Serial.print(F("Temperature(°C), Humidity(%), "));
  if(bmp.id) Serial.print(F("Pressure(mmHg), Temperature2(°C), "));
  Serial.print(F("("));
  Serial.print(READ_DELAY_MS);
  Serial.println(FF(MS_MSG_DELAY_END));

  // Main loop
  while(1){
    core.moveCursor(0, 1);
    if(aht.status != AHT_NO_DEVICE){ // Read from AHT20
      _aht_read(aht);
      core.printValScale(core, aht.temp / 10); 
      core.print(MS_SYM_DEGREE_CODE);
      core.print(F("C "));
      core.printValScale(core, aht.hum / 10);
      core.print(F("% "));

      core.printValScale(Serial, aht.temp / 10);
      Serial.print(", ");      
      core.printValScale(Serial, aht.hum / 10);
      Serial.print(", ");            
    }//if

    if(bmp.id){ // Read from DMP280
      int32_t temp = _bmp_temp(bmp);
      uint32_t pres = _bmp_pres(bmp);
      core.print(pres / 133); 
      core.print(F("mmHg T2:"));
      core.printValScale(core, temp / 10); 
      core.print(MS_SYM_DEGREE_CODE);
      core.println(F("C"));

      Serial.print(pres / 133);
      Serial.print(", ");
      core.printValScale(Serial, temp / 10);
      Serial.print(", ");
    }//if
    Serial.println();  
    delay(READ_DELAY_MS);
  }//while  
  
}//plgBMP280AHT20
