#include "plgDS18B20.h"
#include "uOneWire.h"

#define WORK_PIN P0
#define READ_DELAY_MS 2000 // 2 seconds betwen reading
#define DS18B20_CODE 0x28  // Family code for DS18B20

// Команды DS18B20 
#define SKIP_ROM         0xCC //Пропустить выбор устройства по адресу (обратиться ко всем устройствам сразу)
#define READ_SCRATCHPAD  0xBE //Прочесть данные
#define CONVERT          0x44 //Получить температуру

/*Точность из поля conf: 
* 0x1f - сдвиг на 1
* 0x3f - сдвиг на 2
* 0x5f - сдвиг на 3
* 0x7f - сдвиг на 4
*/
#define CONF2SHIFT(x) (((x >> 4) + 1) >> 1)

namespace DS18B20 {

// Структура Scratchpad
typedef struct {
 uint8_t tempLSB; // Младший байт температуры
 uint8_t tempMSB; // Старший байт температуры
 uint8_t tH; // Регистр тревоги верхнего предела
 uint8_t tL; // Регистр тревоги нижнего предела
 uint8_t conf; // Регистр конфигурации
 uint8_t res1; // Зарезервировано (FF)
 uint8_t res2; // Зарезервировано (0C)
 uint8_t res3; // Зарезервировано (10)
 uint8_t crc; // CRC
} Scratchpad;


uint8_t _read(int16_t &temp, uint8_t &conf){
  Scratchpad data;
  
  if(!uOW_reset(WORK_PIN)) return 0; // Bus reset error

  uOW_exchangeByte(WORK_PIN, SKIP_ROM);//Send SKIP_ROM. Because we have only ONE connected sensor
  uOW_exchangeByte(WORK_PIN, CONVERT); //Send CONVERT to request temperature from the sensor

  digitalWrite(WORK_PIN, HIGH); //Set HIGH because the sensor needs some power to perform conversion
  delay(800); //Maximum time to execute CONVERT command

  if(!uOW_reset(WORK_PIN)) return 0; // Bus reset error
  
  uOW_exchangeByte(WORK_PIN, SKIP_ROM);//Send SKIP_ROM. Because we have only ONE connected sensor
  uOW_exchangeByte(WORK_PIN, READ_SCRATCHPAD); //Send READ_SCRATCHPAD to get the data
  uOW_readBuf(WORK_PIN, (uint8_t*)&data, sizeof(data)); // Read the data
  
  temp = (data.tempMSB << 8) | data.tempLSB; 
  conf = data.conf;
  
  return data.crc == uOW_crc((uint8_t*)&data, sizeof(data) - 1); // CRC check
}//_read

} //namespace

using namespace DS18B20;

// == Main plugin function ==
void plgDS18B20(){
  // Init 
  uOW_ROM rom;
  uOW_Errors err;

  core.moveCursor(0, 1);
  
  if(!uOW_readROM(WORK_PIN, rom, err)){ // readROM error
    uOW_printError(core, err);
    core.println();
    uOW_printError(Serial, err);
    Serial.println();
    while(1); // stop until reboot
  }//if

  if(rom.familyCode != DS18B20_CODE){ // Wrong family code
    uOW_printError(core, WRONG_FAMILY_CODE, rom.familyCode);
    core.println();
    uOW_printError(Serial, WRONG_FAMILY_CODE, rom.familyCode);
    Serial.println();
    while(1); // stop until reboot   
  }//if
  
  int16_t temp;
  uint8_t conf;
  uint8_t need_header = 1; 
  
  Serial.print(F("S/N: "));
  core.printHexArray(Serial, rom.serialNumber, arraySize(rom.serialNumber)); 
  Serial.println();
  
  // Main loop
  while(1){
    core.moveCursor(0, 1);
    
    // Request temperature from the sensor
    if(_read(temp, conf)){ 
      // Read successfull
      core.print(F("T:"));
      core.printValScale(core, (temp * 10) >> CONF2SHIFT(conf));
      core.print(MS_SYM_DEGREE_CODE);
      core.print(F("C SN:"));
      core.printHexArray(core, rom.serialNumber, arraySize(rom.serialNumber)); // Print S/N
      core.println();

      if(need_header){ // Need to print header for serial plotter
        Serial.print(F("Temperature(°C), ("));
        Serial.print(READ_DELAY_MS);
        Serial.println(FF(MS_MSG_DELAY_END));
        need_header = 0;
      }//if
      core.printValScale(Serial, (temp * 10) >> CONF2SHIFT(conf));
      Serial.println(" ");
    }//if
    else {
      core.printHexArray(core, rom.serialNumber, arraySize(rom.serialNumber));
      core.println();

      Serial.println(FF(MS_MSG_READ_ERROR));  // Print error message to the serial  
      need_header = 1; //Next time print header again
    }//if..else
  
    delay(READ_DELAY_MS);  
  }//while
}//
