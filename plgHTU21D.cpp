#include "plgHTU21D.h"
#include <Wire.h>


#define SDA_PIN P6 //Yellow-Black
#define SCL_PIN P7 //Gray-Black
#define READ_DELAY_MS 500 // 0.5 seconds between attempt

#define HTU_ADDRESS 0x40 // HTU21D I²C address 

namespace HTU21D { 
  
#define HTU21_CMD_READ_TEMP 0xE3 // Read temperature command
#define HTU21_CMD_READ_HUM  0xE5 // Read humidity command  
#define HTU21_CMD_WRITE_REG 0xE6 // Write register command  
#define HTU21_CMD_READ_REG  0xE7 // Read register command  
#define HTU21_CMD_RESET     0xFE // Reset command  


 uint8_t _init(){
  // Сбрасываем устройство
  core.i2cWriteReg(HTU_ADDRESS, HTU21_CMD_RESET, 0, SIZE_0); // Просто даем команду без аргументов (третий параметр = 0 не передается)
  delay(15); // Ждем пока сбросится
  // Отправляем READ_REG. Должно вернуть 0x02.
  return core.i2cReadReg(HTU_ADDRESS, HTU21_CMD_READ_REG) == 0x02;
 }//_init


 uint8_t _is_CRC_OK(uint16_t data){
  for(uint8_t i = 0; i < 16; i++){
    if(data & 0x8000) data = (data << 1) ^ 0x13100;
    else data <<= 1;
  }//for 
  return data >> 8;
 }//_is_CRC_OK


 uint32_t _read_val(const uint8_t cmd){
  //Отправляем команду на получение данных
  core.i2cWriteReg(HTU_ADDRESS, cmd, 0, SIZE_0);
  //Ждем пока данные будут подготовлены
  delay(50);
  // Читаем 3 байта от устройства. Значение (2 байта) + CRC (1 байт)
  Wire.requestFrom(HTU_ADDRESS, 3);
  uint16_t data = (Wire.read() << 8) | Wire.read();
  if(_is_CRC_OK(data) != Wire.read()) data = 0xFFFF; // CRC не сошлось
  return data;
 }//_read_val


 int16_t _get_temp(const uint32_t val){ // temp * 100
  return (uint16_t)(((val * 17572) >> 16) - 4685);
 }//_get_temp


 int16_t _get_hum(const uint32_t val){ // hum * 100
  return constrain((int16_t)(((val * 12500) >> 16) - 600), 0, 10000);
 }//_get_hum
} //namespace


using namespace HTU21D;
// == Main plugin function ==
void plgHTU21D(){
  // Init
  uint8_t need_header = 1;
  Wire.begin();     
  if(!_init()) Serial.println(FF(MS_MSG_READ_ERROR));
  
  // Main loop
  while(1){
    core.moveCursor(0, 1);

    // Читаем температуру
    uint32_t value = _read_val(HTU21_CMD_READ_TEMP);
    if(value != 0xFFFF){ // Выводим температуру
      core.print(F("T:"));
      core.printValScale(core, _get_temp(value) / 10); 
      core.print(MS_SYM_DEGREE_CODE);
      core.print(F("C H:"));

      core.printValScale(Serial, _get_temp(value) / 10);
      Serial.print(", ");       
    }//if 
    else{
      core.println(FF(MS_MSG_READ_ERROR));
      Serial.println(FF(MS_MSG_READ_ERROR));        
      need_header = 1;      
    }//if..else

    // Читаем и выводим влажность
    value = _read_val(HTU21_CMD_READ_HUM);
    if(value != 0xFFFF){
      core.printValScale(core, _get_hum(value) / 10);
      core.println(F("%"));
      
      core.printValScale(Serial, _get_hum(value) / 10);
      Serial.println();
    }//if
    else{
      core.println(FF(MS_MSG_READ_ERROR));
      Serial.println(FF(MS_MSG_READ_ERROR));        
      need_header = 1;      
    }//if..else

    if(need_header){
      Serial.print(F("Temperature(°C), Humidity(%), ("));
      Serial.print(READ_DELAY_MS);
      Serial.println(FF(MS_MSG_DELAY_END));
      need_header = 0;
    }//if
    
    delay(READ_DELAY_MS);    
  }//while  
}//plgHTU21D
