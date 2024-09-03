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

#include "plgTM1637.h"

#define DIO_PIN P0
#define CLK_PIN P1
#define MAX_DIGITS 4 // From 4 to 6
#define STEP_DELAY_MS 250 

#define TM1637_START_ADDR 0xC0 // Стартовый адрес памяти
#define TM1637_AUTO_ADDR 0x40  // Автоматически увеличивать адрес
#define TM1637_FIXED_ADDR 0x44 // Не увеличивать адрес (записать по фиксированному адресу)
#define TM1637_DISPLAY_ON 0x88 // Включить дисплей (сюда еще добавляется яркость от 0 до 7)


namespace TM1637 {
  // Начать передачу данных
  void _beginTransmission(void){
    digitalWrite(CLK_PIN, HIGH);
    digitalWrite(DIO_PIN, HIGH);
    digitalWrite(DIO_PIN, LOW);    
    digitalWrite(CLK_PIN, LOW);  
  }//_beginTransmission


  // Закончить передачу данных
  void _endTransmission(void){
    digitalWrite(CLK_PIN, LOW);
    digitalWrite(DIO_PIN, LOW);
    digitalWrite(CLK_PIN, HIGH);
    digitalWrite(DIO_PIN, HIGH);    
  }//_endTransmission


  // Передать байт
  uint8_t _transmit(uint8_t data){
    for(uint8_t i = 0; i < 8; i++){ // Передаем побитно
      digitalWrite(CLK_PIN, LOW);
      digitalWrite(DIO_PIN, data & 0x01); // Выставляем правый бит
      data >>= 1; // Смещаем вправо
      digitalWrite(CLK_PIN, HIGH);
    }//for

    // Ждем ASK
    digitalWrite(CLK_PIN, LOW);
    digitalWrite(DIO_PIN, HIGH);
    digitalWrite(CLK_PIN, HIGH);
    pinMode(DIO_PIN, INPUT);
    delayMicroseconds(50);
    uint8_t ack = digitalRead(DIO_PIN); // Читаем ACK
    pinMode(DIO_PIN, OUTPUT);
    if(!ack) digitalWrite(DIO_PIN, LOW);
    delayMicroseconds(100);
    return ack;
  }//_transmit


  // Передать код символа в заданное знакоместо
  void _sendSymCode(uint8_t pos, const uint8_t code){
    pos = constrain(pos, 0, MAX_DIGITS - 1); // Знакоместо в диапазоне от 0 до MAX_DIGITS
   _beginTransmission();
   _transmit(TM1637_FIXED_ADDR);  // Будем работат без автоинкремента
   _endTransmission();
  
   _beginTransmission();
   _transmit(TM1637_START_ADDR | pos); // Отправляем адрес знакоместа
   _transmit(code); // Отправляем данные
   _endTransmission();

   _beginTransmission();
   _transmit(TM1637_DISPLAY_ON + 7); // Отправляем команду управления + яркость
   _endTransmission();    
  }//_sendSymCode

  
  void _clear(){
    for(uint8_t i = 0; i < MAX_DIGITS; i++) _sendSymCode(i, 0);
  }// _clear  


  void _stepForward(uint8_t &pos, uint8_t &code){  
    if(code == 0xFF){       
      pos++;
      code = 0;
    }//if
    else code = (code << 1) | 0x01;
    
    if(pos == MAX_DIGITS){ 
      pos = 0;
      code = 0;
    }//if   
    _sendSymCode(pos, code);    
  }//_stepForward


  void _stepBackward(uint8_t &pos, uint8_t &code){  
    if(!code){       
      pos--;
      code = 0xFF;
    }//if
    else code >>= 1;
    
    if(pos > MAX_DIGITS){ 
      pos = MAX_DIGITS;
      code = 0xFF;
    }//if
    _sendSymCode(pos, code);    
  }//_stepBackward


  void _display(const uint8_t pos, const uint8_t code){
    Serial.print(F("Position: "));
    Serial.print(pos);
    Serial.print(F(" Code: 0b"));
    Serial.print(core.rAlign(code, 8, '0', BIN));
    Serial.print(F(" (0x"));
    Serial.print(core.rAlign(code, 2, '0', HEX));
    Serial.println(')');
    
    core.moveCursor(0, 1);    
    core.print(F("P:"));
    core.print(pos);
    core.print(F(" C:0b"));
    core.println(core.rAlign(code, 8, '0', BIN));    
  }//_display
} //namespace

using namespace TM1637;

// == Main plugin function ==
void plgTM1637(){
  // Init
  pinMode(DIO_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  _clear();

  uint8_t pos = 0;
  uint8_t code = 0;
  uint8_t is_auto = 1;

  // Display Init

  // Main loop
  while(1){
    // Process user input    
    switch (core.getButton()) {
      case SELECT: is_auto = !is_auto; break; // auto or manual mode
      case SELECT_LONG: _clear(); pos = 0; code = 0; _display(pos, code); break; // Clear and start from the begin
      case UP: _stepBackward(pos, code); _display(pos, code); break; // Step backward   
      case DOWN: _stepForward(pos, code); _display(pos, code); break; // Step froward
      default: break;
    }//switch        

    if(!is_auto) continue; // Auto mode
    _stepForward(pos, code);
    _display(pos, code);
    delay(STEP_DELAY_MS);
  }//while
}//plgTM1637
