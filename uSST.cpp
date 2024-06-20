#include "uSST.h"
#include "mscore.h"

//** Читает строку из Serial в буфер до появления /n, /r или таймаута. Отрезает /n, /r. Ставит ноль в конце. 
uint8_t uSST_ReadString(const SoftwareSerial &ser, char* buf, const uint8_t buf_size, const uint32_t timeout_ms){
  uint8_t i = 0;
  uint32_t t_start = millis();
  while((millis() - t_start) < timeout_ms){
    while(ser.available()){
      buf[i] = ser.read();
      if((buf[i] == 0x0D) || (buf[i] == 0x0A) || (i == (buf_size - 1))){ // Дошли до /n или /r или до конца буфера
        buf[i] = 0;
        return i;
      }//if
      i++;
    }//while
  }//while
  buf[i] = 0;
  return i;
}//uSST_ReadString


//** Последовательно открыват порт на разных скоростях (от большей к меньшей) и вызывает функцию проверки. 
//*  Если проверка прошла - возвращает скорость порта. Если ни одной проверка не прошло - возвращает 0.
uint32_t uSST_FindSpeed(const SoftwareSerial &ser, uSST_ProbeFunction probe, char* buf, const uint8_t buf_size, const uint32_t timeout_ms){
  const uint32_t speed[] = {115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200};  
  for(uint8_t i = 0; i < arraySize(speed) - 1; i++){
    ser.begin(speed[i]);
    if(probe(ser, buf, buf_size, timeout_ms)){
      ser.end();
      return speed[i];
    }//if
    ser.end();
  }//for
  return 0;
}//uSST_FindSpeed


//** Ищет n-ое входжение символа в строку. Возвращает индекс или 0, если нужного входжения не найдено 
uint8_t uSST_strchrn(char* str, const char sym, const uint8_t n){
  uint8_t i = 0;
  uint8_t cnt = 0;
  while(str[i]){ // До финального нуля
    if(str[i] == sym) cnt++;
    if(cnt == n) return i;
    i++;
  }//while
  return 0;
}//uSST_strchrn
