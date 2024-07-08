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

#include "plgHX711.h"


#define DT_PIN P0
#define SCK_PIN  P1
#define READ_DELAY_MS 500 // 0.5 seconds between attempt

namespace HX711 {
  
  // 1 - если новые данные готовы, иначе 0
  _is_ready(){
    return !digitalRead(DT_PIN);
  }//is_ready


  // Прочесть 
  int32_t _read(){
    union { // Совмещаем 4-х байтовый массив с 32-битным целым
      int32_t value = 0;
      uint8_t data[4];
    } val;

    noInterrupts();
    // Читаем 24 бита в обратном порядке (от старшего байта)
    val.data[2] = shiftIn(DT_PIN, SCK_PIN, MSBFIRST);
    val.data[1] = shiftIn(DT_PIN, SCK_PIN, MSBFIRST);
    val.data[0] = shiftIn(DT_PIN, SCK_PIN, MSBFIRST);

    // Для усиления 128 на канале А нужна одна послыка
    digitalWrite(SCK_PIN, HIGH);
    digitalWrite(SCK_PIN, LOW);
    interrupts();

    // Отрицательные значения
    if(val.data[2] & 0x80) val.data[3] = 0xFF;
    return val.value;
  }//_read


  // Подождать значение и прочесть
  int32_t _w_read(){
    while(!_is_ready()) yield();
    return _read();
  }//_w_read


  // Калибровать
  int32_t _get_tare(const uint8_t cnt = 16){
    int32_t res = 0;    
    for(uint8_t i = 0; i < cnt; i++) res += _w_read();
    return res >> 4;
  }//_get_tare


  // Получить значение в граммах с учетом тары и коэффицента
  int32_t _get_gramm(const int32_t tare){
    return (_w_read() - tare) / plgHX711Cfg.coef;
  }//_get_gramm
} //namespace


using namespace HX711;


// == Main plugin function ==
void plgHX711(){
  // Init
  pinMode(DT_PIN, INPUT);
  pinMode(SCK_PIN, OUTPUT);
  digitalWrite(SCK_PIN, LOW);  
 
  int32_t tare = _get_tare(); // Фиксация нулевого уровня
  int32_t weight = _get_gramm(tare); // Начальный вес   
  
  // Load settings from EEPROM 
  if(!core.loadSettings((uint8_t*)&plgHX711Cfg)){
    plgHX711Cfg.coef = 1;
    core.saveSettings((uint8_t*)&plgHX711Cfg);// Save default value  
  }//if  
 
  // Display Init
  core.moveCursor(0, 1);  
  core.print(F("W: "));
  core.print(weight);
  core.print(F("g (k:"));
  core.print(plgHX711Cfg.coef);
  core.println(F(")"));

  Serial.print(F("K: "));
  Serial.println(plgHX711Cfg.coef);
  Serial.print(F("Weight(g), ("));
  Serial.print(READ_DELAY_MS);
  Serial.println(FF(MS_MSG_DELAY_END));
  
  // Main loop
  while(1){
    // Process user input    
    switch (core.getButton()) {
      // Увеличение коэффициента пропорциональности 
      case UP: plgHX711Cfg.coef ++; break;
      case UP_LONG: plgHX711Cfg.coef +=10; break;

      // Умкеньшение коэффициента пропорциональности 
      case DOWN: plgHX711Cfg.coef --; break;
      case DOWN_LONG: plgHX711Cfg.coef -=10; break;
      
      case SELECT: tare = _get_tare(); break; // Установка тары
      
      case SELECT_LONG: core.saveSettings((uint8_t*)&plgHX711Cfg);break;   // save settings to EEPROM
      default: break;
    }//switch

    // Получаем текущий вес
    if(weight == _get_gramm(tare)) continue; // вес не изменился
    weight = _get_gramm(tare);
    
    core.moveCursor(0, 1);
    core.print(F("W: "));
    core.print(weight);
    core.print(F("g (k:"));
    core.print(plgHX711Cfg.coef);
    core.println(F(")"));

    Serial.println(weight);
    delay(READ_DELAY_MS);  
  }//while
}//plgHX711
