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

#define BMP_RESET_REG 0x0e // Reset register
#define BMP_RESET_VAL 0xb6 // Reset value

#define BMP_ID_REG 0xd0 // ID register

namespace BMP280AHT20 {
  // BMP(E) settings 
  typedef struct {
    uint8_t  id; // BMP/BME ID. Zero if no device present
    uint16_t t1;
    int16_t  t2;
    int16_t  t3;
    uint16_t p1;
    int16_t  p2;
    int16_t  p3;
    int16_t  p4;
    int16_t  p5;
    int16_t  p6;
    int16_t  p7;
    int16_t  p8;
    int16_t  p9;
    uint16_t h1; // For BME
    int16_t  h2;
    uint8_t  h3;
    int16_t  h4;
    int16_t  h5;
    int8_t   h6;
  } BMPSettings;

  
  // I2C Register operations
  uint8_t _read_reg8(const uint8_t i2c_addr, const uint8_t reg){
    Wire.beginTransmission(i2c_addr);
    Wire.write(reg);
    if(!Wire.endTransmission()) return 0; // Transmission error
    Wire.requestFrom(i2c_addr, 1);
    return Wire.read();
  }//_read_reg8


  uint8_t _write_reg8(const uint8_t i2c_addr, const uint8_t reg, uint8_t val){
    Wire.beginTransmission(i2c_addr);
    Wire.write(reg);
    Wire.write(val);
    return !Wire.endTransmission();
  }//_write_reg8


  void _bmp_init(BMPSettings &result, const uint8_t i2c_addr = BMP_ADDRESS){
    result.id = 0;
   
    // Trying to reset sensor
    if(!_write_reg8(i2c_addr, BMP_RESET_REG, BMP_RESET_VAL)) return;

    delay(10);

    // Не хочет!! отдает 0
    result.id = _read_reg8(i2c_addr, BMP_ID_REG);  
    
    Serial.println(result.id, HEX);
    
    if((result.id != BMP_ID) && (result.id != BME_ID)){
      result.id = 0;
      return;
    }//if

    // Дальше читаем калибровку и пишем настройки
  }//_bmp_init
  
} //namespace

using namespace BMP280AHT20;


// == Main plugin function ==
void plgBMP280AHT20(){
  // Init
  Wire.begin();  
  BMPSettings bmp;
  _bmp_init(bmp);  
 
  // Main loop
  while(1){

  }//while  
  
}//plgBMP280AHT20
