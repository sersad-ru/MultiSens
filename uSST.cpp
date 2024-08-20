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

#include "uSST.h"
#include "mscore.h"


//** Последовательно открыват порт на разных скоростях (от большей к меньшей) и вызывает функцию проверки. 
//*  Если проверка прошла - возвращает скорость порта. Если ни одной проверка не прошло - возвращает 0.
uint32_t uSST_FindSpeed(SoftwareSerial &ser, uSST_ProbeFunction probe, char* buf, const uint8_t buf_size, const uint32_t timeout_ms, const uint32_t guess_speed){
  // Проверяем соединение на предполагаемой скорости
  if(guess_speed){
    ser.begin(guess_speed);
    if(probe(ser, buf, buf_size, timeout_ms)){
      ser.end();
      return guess_speed;
    }//if
    ser.end();    
  }//if
  
  //Не подошло. Перебираем все скорости по списку.
  const uint32_t speed[] = {115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200};  
  for(uint8_t i = 0; i < arraySize(speed); i++){
    ser.begin(speed[i]);
    if(probe(ser, buf, buf_size, timeout_ms)){
      ser.end();
      return speed[i];
    }//if
    ser.end();
  }//for
  return 0;
}//uSST_FindSpeed


//** Ищет n-ое входжение символа в строку. Возвращает индекс или -1, если нужного входжения не найдено 
int8_t uSST_strchrn(char* str, const char sym, const uint8_t n){
  uint8_t i = 0;
  uint8_t cnt = 0;
  while(str[i]){ // До финального нуля
    if(str[i] == sym) cnt++;
    if(cnt == n) return i;
    i++;
  }//while
  return -1;
}//uSST_strchrn
