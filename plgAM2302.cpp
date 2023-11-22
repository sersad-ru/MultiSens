#include "plgAM2302.h"

#define DATA_PIN P0
#define READ_DELAY_MS 2000 // 2 seconds betwen reading

// Timings (in us)
#define START_SIGNAL 1000 //1ms   Начальное прижатие шины к нулю
#define RELEASE_BUS    30 //30us  Начальное отпускание шины
#define RESPONSE_LOW   80 //80us  Ответ датчика нижним уровнем
#define RESPONSE_HIGH  80 //80us  Ответ датчика верхним уровнем
#define BIT_PREFIX     50 //50us  Префикс очередного бита
#define BIT_ZERO_ONE   45 //45us  Пороговое значение отличающее передачу нуля от единицы. Ноль [22; 30], единица [68;75] 
#define BIT_TIMEOUT   100 //100us Таймаут при передаче битов

uint8_t _get_time(uint8_t level, uint8_t timeout){
  uint32_t s = micros();
  while(digitalRead(DATA_PIN) == level){
    if((micros() - s) > timeout) return 0;
  }//while
  return micros() - s;
}// _get_time


uint8_t _read_byte(){
  int8_t res = 0;
  for(uint8_t i = 0; i < 8; i++){
    _get_time(LOW, BIT_TIMEOUT);
    res = (res << 1) | (_get_time(HIGH, BIT_TIMEOUT) > BIT_ZERO_ONE ? 1 : 0); //Если больше порога - это "1" иначе "0"
  }//for
  return res;
}//_read_byte


uint8_t _read(int16_t &tmp, int16_t &hum){
  // Pull down bus for a 1ms
  pinMode(DATA_PIN, OUTPUT);
  digitalWrite(DATA_PIN, LOW);
  delayMicroseconds(START_SIGNAL);

  // Release buss for 40 us
  digitalWrite(DATA_PIN, HIGH);
  delayMicroseconds(RELEASE_BUS);

  // Read the bus
  pinMode(DATA_PIN, INPUT);
  if(_get_time(LOW, RESPONSE_LOW) == 0) return 0; // Read error
  if(_get_time(HIGH, RESPONSE_HIGH) == 0) return 0; // Read error

  // Read the humidity
  hum = (_read_byte() << 8 ) | _read_byte(); //High and low bytes of humidity

  // Starting CRC calculation
  uint8_t crc = (hum & 0xff) + (hum >> 8);

  // Read the temperature
  uint8_t t = _read_byte(); // High byte of temperature
  tmp = ((t & 0x7f) << 8) | _read_byte(); // cut off high bit, shift left 8 times and add low byte
  crc += t + (tmp & 0xff); // We need full high byte for correct CRC
  if(t & 0x80) tmp = -tmp; // Temperature is negative if high bit was set

  return crc == _read_byte(); // Compare calculated CRC with CRC from sensor
}//_read


void _print_val(Print &p, int16_t val){
  p.print(val / 10);
  p.print(F("."));
  p.print(abs(val % 10));
}//_print_val


// == Main plugin function ==
void plgAM2302(){
  //Init
  int16_t tmp = 0;
  int16_t hum = 0;  

  Serial.print(F("Temperature(°C), Humidity(%), ("));
  Serial.print(READ_DELAY_MS);
  Serial.println(FF(MS_MSG_DELAY_END));
  //Main loop
  while(1){
    if(_read(tmp, hum)){
      core.moveCursor(0, 1);
      core.print(F("T:"));
      _print_val(core, tmp);
      core.print(MS_SYM_DEGREE_CODE);
      core.print(F("C H:"));
      _print_val(core, hum);
      core.println(F("%"));
      
      _print_val(Serial, tmp);
      Serial.print(", ");      
      _print_val(Serial, hum);
      Serial.println();
    }//if
    else{
      core.moveCursor(0, 1);
      core.println(FF(MS_MSG_READ_ERROR));
      Serial.println(FF(MS_MSG_READ_ERROR));
    }//if..else
    
    delay(READ_DELAY_MS);
  }//while
}//plgAM2302
