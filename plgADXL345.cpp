#include "plgADXL345.h"
#include <Wire.h>


#define SDA_PIN P6 //Ж
#define SCL_PIN P7 //С
#define READ_DELAY_MS 500 // 0.5 seconds between attempt

#define ADXL_ADDRESS 0x53  // ADXL I2C address
#define POWER_CTL 0x2D //Power register
#define MEASURE_MASK 0x08 // Set D3 bit to 1 to start measure

#define DATA_FORMAT 0x31 // Data format register
#define RANGE_MASK_16 0x0b // 16g range (b) Z: 50 = +1 40 = 0 30 = -1. 
/*
 * Разобраться с осью Z
 * При разрешении 2g не работает. При 4/8/16 - работает
 * На 16g:
 * +1 - 50 (1300)
 * 0  - 40 (1030)
 * -1 - 30 (800)
 * 
 * На 8g:
 * +1  - 1300
 * 0   - 1030
 * -1  - 800
 * 
 * Сделать калибровку по <SELECT>
 * Калибровка (https://microkontroller.ru/arduino-projects/opredelenie-orientaczii-s-pomoshhyu-akselerometra-adxl345-i-arduino/?ysclid=lpl423o8ly906304703)
 * Ну и datasheet глянуть и adafruit
 * 
 * Домножение по adafruit: 9.80665 * 0.004 ?
*/


#define DATA_REG_START 0x32 // First data register
#define DATA_REG_SIZE 0x06  // Size of data register block

namespace ADXL345 {
typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
} acc;  

} //namespace


using namespace ADXL345;

// == Main plugin function ==
void plgADXL345(){
  // Init
  Wire.begin();

  // Switch sensor to measurement mode
  Wire.beginTransmission(ADXL_ADDRESS);
  Wire.write(POWER_CTL);
  Wire.write(MEASURE_MASK);
  Wire.endTransmission();
  delay(10);

  
  Wire.beginTransmission(ADXL_ADDRESS);
  Wire.write(DATA_FORMAT);
  Wire.write(RANGE_MASK_16);
  Wire.endTransmission();
  
  Wire.beginTransmission(ADXL_ADDRESS);
  Wire.write(DATA_FORMAT);
  Wire.requestFrom(ADXL_ADDRESS, 1, true);
  Serial.println(Wire.read(), HEX);
  

  
  
  
  // Main loop
  while(1){
    Wire.beginTransmission(ADXL_ADDRESS);
    Wire.write(DATA_REG_START);
    Wire.endTransmission(false);
    
    Wire.requestFrom(ADXL_ADDRESS, DATA_REG_SIZE, true);
    int16_t X = Wire.read() | Wire.read() << 8; //LSB | HSB
    int16_t Y = Wire.read() | Wire.read() << 8; //LSB | HSB
    int16_t Z = Wire.read() | Wire.read() << 8; //LSB | HSB

    //Wire.endTransmission();

    Serial.print("X: ");
    Serial.println(X);
    //Serial.println(X / (float)64);
    
    Serial.print("Y: ");
    Serial.println(Y);
    //Serial.println(Y / (float)64);
    
    Serial.print("Z: ");
    Serial.println(Z);
 //   Serial.println(Z / (float)64);

    Serial.println();
   
    delay(READ_DELAY_MS);
  }//while  
}//plgADXL345
