#include "plgMPU6050.h"
#include <Wire.h>


#define SDA_PIN P6 //Yellow-Black
#define SCL_PIN P7 //Gray-Black
#define READ_DELAY_MS 500 // 0.5 seconds between attempt
#define MPU_ADDRESS  0x68 // MPU6050 I²C address 


#define REG_POWER_1    0x6B // Регистр управления питанием
#define REG_WHO_AM_I   0x75 // Регист идентификатора устройства
#define REG_DATA_START 0x3B // Начальный регистр блока данных

#define DEVICE_ID     0x68 // Значение регистра удентификатора устройства
namespace MPU6050 {
  
  typedef struct {
    int8_t accX; // Ускорение по оси X (диапазон -2..2)g, умноженное на 100. [-200..200]  
    int8_t accY;
    int8_t accZ;
    int16_t temp; // Температура в градусах цельсия, умноженная на 100
    int16_t gyrX; // Угловая скорость по оси X в градусах в секунду (диапазон -250..250)
    int16_t gyrY;
    int16_t gyrZ;
  } dataPkt;
  
  uint8_t _init(){
    //Выводим из режима сна и устанавливаем синхронизацию по внутренему источнику
    core.i2cWriteReg(MPU_ADDRESS, REG_POWER_1, 0);
    return core.i2cReadReg(MPU_ADDRESS, REG_WHO_AM_I) == DEVICE_ID; // Читаем ID и убеждаемся, что он тот 
  }//_init


  dataPkt _read(){
    dataPkt data;
    core.i2cRequestRead(MPU_ADDRESS, REG_DATA_START, 14); // Читаем пакет даннх 14 байт  HSB | LSB
    data.accX = (((int32_t)((Wire.read() << 8) | Wire.read())) * 100) >> 14; // Умножаем на 100 и потом делитим на 16384
    data.accY = (((int32_t)((Wire.read() << 8) | Wire.read())) * 100) >> 14;
    data.accZ = (((int32_t)((Wire.read() << 8) | Wire.read())) * 100) >> 14;
    data.temp = (((int32_t)((Wire.read() << 8) | Wire.read())) * 10) / 34 + 3653; //temp / 340 + 36.53, умноженное на 100
    data.gyrX = (((int32_t)((Wire.read() << 8) | Wire.read())) * 250) >> 15; // val * 250 / 32768
    data.gyrY = (((int32_t)((Wire.read() << 8) | Wire.read())) * 250) >> 15;
    data.gyrZ = (((int32_t)((Wire.read() << 8) | Wire.read())) * 250) >> 15;
    // преобразовываем данные 
    return data;
  }//_read

  void _print_val(const char name, const int16_t val, const uint8_t direct = 0){
    core.print(name);
    core.print(':');
    if(direct){ 
      core.print(val); // Прямая печать, без огруглений и пересчетов
      Serial.print(val);
    }//..if
    else{
      core.printValScale(core, (val + 5) / 10); // одна цифра после запятой для экрана (с округлением)
      core.printValScale(Serial, val, 100); // две цифры после запятой для сериала      
    }//if..else
    core.print(' ');    
    Serial.print(' ');
  }//_print_val  
} //namespace


using namespace MPU6050;

// == Main plugin function ==
void plgMPU6050(){
  // Init
  Wire.begin();     
  core.moveCursor(0, 1);
  
  if(!_init()){
    core.println(FF(MS_MSG_READ_ERROR));
    while(1);
  }//if
    

  dataPkt data;
  // Main loop
  while(1){
    data = _read();
    _print_val('X', data.accX);
    _print_val('Y', data.accY);
    _print_val('Z', data.accZ);

    _print_val('A', data.gyrX, true);
    _print_val('B', data.gyrY, true);
    _print_val('C', data.gyrZ, true);

    _print_val('T', data.temp);

    core.println();
    Serial.println();
    
    /*
    Serial.print(data.accX);
    Serial.print(", ");
    Serial.print(data.accY);
    Serial.print(", ");
    Serial.println(data.accZ);
    
    _print_val('T', data.temp);
    Serial.println(data.temp);
    
    Serial.print(data.gyrX);
    Serial.print(", ");
    Serial.print(data.gyrY);
    Serial.print(", ");
    Serial.println(data.gyrZ);
*/    
    delay(READ_DELAY_MS);
  }//while  
}//plgMPU6050
