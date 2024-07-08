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

#include "plgRC522.h"
#include "mscardmsg.h"
#include <SPI.h>

#define SDA_PIN  P0 //Green //SS
#define RST_PIN  P1 //Blue
#define MOSI_PIN P2 //Yellow
#define MISO_PIN P3 //Violet
#define SCK_PIN  P4 // Brown

#define READ_DELAY_MS 500 // 0.5 seconds between attempt


// Registers
#define COMMAND_REG     0x01 // Регистр команд
#define T_MODE_REG      0x2A // Регистры настройки таймера
#define T_PRESCALER_REG 0x2B // Регистры настройки таймера
#define T_RELOAD_REG_H  0x2C // Старший байт времени перезагрузки таймера
#define T_RELOAD_REG_L  0x2D // Младший байт времени перезагрузки таймера
#define TX_ASK_REG      0x15 // Регистр настроек TX модуляции 
#define MODE_REG        0x11 // Регистр режимов приема и передачи
#define TX_CONTROL_REG  0x14 // Регистр управления антеной
#define COMM_I_EN_REG   0x02 // Регист разрешения и запрещения прерываний
#define COMM_IRQ_REG    0x04 // Регистр битов запросов прерываний
#define FIFO_LEVEL_REG  0x0A // Количество байт в FIFO
#define FIFO_DATA_REG   0x09 // FIFO буфер
#define BIT_FRAMING_REG 0x0D // Корректировка битовых кадров
#define ERROR_REG       0x06 // Результат (ошибки) выполнения последней команды
#define CONTROL_REG     0x0C // Различные управляющие биты
#define VERSION_REG     0x37 // Текущая версия

// Commands
#define CMD_RESET      0x0F // Команда сброса
#define CMD_TRANSCEIVE 0x0C // Отправить данные из FIFO на антену и начать прием данных с антены
#define CMD_IDLE       0x00 // Отменить текущую команду
#define CMD_REQ_IDL    0x26 // Перейти в режим ожидания
#define CMD_SELECT_TAG 0x93 // Поулчение ID (Антиколлизия. Может быть 0x93, 0x95 и 0x97. В буфере потом следующим байтом передать 0x20)

namespace RC522 { 


 void _write(const uint8_t reg, const uint8_t val){
  digitalWrite(SDA_PIN, LOW);
  SPI.transfer((reg << 1) & 0x7E); // ноль в начале и ноль в конце
  SPI.transfer(val);
  digitalWrite(SDA_PIN, HIGH);
 }//_write


 uint8_t _read(const uint8_t reg){
  uint8_t res;
  digitalWrite(SDA_PIN, LOW);
  SPI.transfer(((reg << 1) & 0x7E) | 0x80); // старший бит ставим в 1
  res = SPI.transfer(0x00);
  digitalWrite(SDA_PIN, HIGH); 
  return res;
 }//_read


 void _set_mask(const uint8_t reg, const uint8_t mask){
  uint8_t val = _read(reg);
  if(!(val & mask)) _write(reg, val | mask);
 }//_set_mask


 void _clear_mask(const uint8_t reg, const uint8_t mask){
  uint8_t val = _read(reg);
  _write(reg, val & (~mask));  
 }//_clear_mask

 
 void _init(){
  // Пины SS(SDA) и RST
  pinMode(SDA_PIN, OUTPUT);
  digitalWrite(SDA_PIN, HIGH); 
  
  pinMode(RST_PIN, OUTPUT);
  digitalWrite(RST_PIN, HIGH); // можно подергать для hard-reset-а
  
/*  
  // Hard reset
  pinMode(RST_PIN, INPUT);
  if(digitalRead(RST_PIN) == LOW){
    pinMode(RST_PIN, OUTPUT);
    digitalWrite(RST_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(RST_PIN, HIGH);
    delay(50);
  }//if
*/  
  // Soft-reset
  _write(COMMAND_REG, CMD_RESET);
  delay(50);  

  //Timer: TPrescaler * TreloadVal / 6.78MHz = 24ms
  _write(T_MODE_REG, 0x8D); // Tauto = 1; Timer = 6.78MHz/TPreScaler = 0x0D
  _write(T_PRESCALER_REG, 0x3E); //TModeReg[3..0] + TPrescalerReg
  _write(T_RELOAD_REG_L, 0x1E);
  _write(T_RELOAD_REG_H, 0x00);
  _write(TX_ASK_REG, 0x40); // 100 % ASK modulation
  _write(MODE_REG, 0x3D); //CalcCRC = 0x6363 Остальное по дефолту (было 0x3F)

  // Antenna On
  _set_mask(TX_CONTROL_REG, 0x03); // Включаем антену
 }//_init


 uint8_t _transfer(uint8_t *sndBuf, const uint8_t sndSize, uint8_t *rcvBuf, uint8_t rcvSize, uint8_t &rcvBits){
  _write(COMM_I_EN_REG, 0xF7); // Разрешаем прерывания 0x77 | 0x80
  _clear_mask(COMM_IRQ_REG, 0x80); // Сбрасываем биты запроса прерывания
  _set_mask(FIFO_LEVEL_REG, 0x80); // Очищаем и инициализируем fifo
  _write(COMMAND_REG, CMD_IDLE); // Отменяем текущую выполняемую команду

  // Отправляем данные
  for(uint8_t i = 0; i < sndSize; i++) _write(FIFO_DATA_REG, sndBuf[i]);

  _write(COMMAND_REG, CMD_TRANSCEIVE);// Даем команду на получение данных
  _set_mask(BIT_FRAMING_REG, 0x80);// Выставляем старший бит, чтобы начать обработку данных из FIFO

  //Ждем завершения получения данных. Максимальное время ожидания 25мс
  int i = 2000;
  for(; i > 0; i--){
    uint8_t res = _read(COMM_IRQ_REG);
    if(res & 0x01) return 0; // Таймаут. Бит 0 = 1 
    if(res & 0x30) break; // Все прошло успешно. Бит 5 = 1 - окончание потока данных & Бит 4 = 1 - выполнение команды завершено. Остальные должны быть = 0
    yield();
  }//for
  if(!i) return 0; // Таймаут
  _clear_mask(BIT_FRAMING_REG, 0x80); // Сбрасываем старший бит

  // Теперь проверяем, что б небыло коллизий и ошибок
  if(_read(ERROR_REG) & 0x1B) return 0; // Были ошибки передачи. Бит 0 - Ошибка протокола, Бит 1 - Ошибка четности, Бит 3 - коллизия, Бит 4 - Переполнение. Остальные пофиг. 

  // Читаем данные, если есть
  uint8_t avail = _read(FIFO_LEVEL_REG);// Сколько байт данных нас ждут
  uint8_t last_bits = _read(CONTROL_REG) & 0x07; //3 младших бита: cколько значащих бит будет в последнем байте. Если = 0, то весь байт значащий.
  rcvBits = avail << 3; // по уполчанию (last_bits == 0) в ответе avail * 8 бит
  if(last_bits) rcvBits = ((avail - 1) << 3) + last_bits; // Сколько всего бит в ответе  
    
  rcvSize = min(avail, rcvSize); // Читаем сколько ждут, но не более, чем размер буфера

  for(i = 0; i < rcvSize; i++) rcvBuf[i] = _read(FIFO_DATA_REG); // Читаем данные из FIFO
  
  return 1; // все OK
 }//_transfer


 uint8_t _is_card_ready(){
  uint8_t rcv_bits;
  uint8_t buf[2];
  uint8_t res;

  buf[0] = CMD_REQ_IDL;
  _write(BIT_FRAMING_REG, 0x07); // В трех младших битах: cколько бит передавать из байта (нам нужно 7). Остальные биты = 0
  res = _transfer(buf, 1, buf, 2, rcv_bits);  
  if(!res) return res; // Вообще ничего не считалось (карты нету)
/*
 * В буфер вернут тип карты:
 * 0x4400 = Mifare_UltraLight
 * 0x0400 = Mifare_One(S50)
 * 0x0200 = Mifare_One(S70)
 * 0x0800 = Mifare_Pro(X)
 * 0x4403 = Mifare_DESFire
*/
//  Serial.println(_read(VERSION_REG), HEX);
  return rcv_bits == 0x10; // Если количеств значащих бит в ответе != 0x10 то тоже все плохо
 }//_is_card_ready


 uint32_t _get_uid(){
  _write(BIT_FRAMING_REG, 0x00); // В трех младших битах: cколько бит передавать из байта (нам нужно 8), значит все три бита = 0 . Остальные биты тоже = 0
  uint8_t buf[] = {CMD_SELECT_TAG, 0x20, 0, 0, 0}; // Буфер предварительно заполнен командой получения UID
  uint8_t rvc_bits;
  uint8_t res = _transfer(buf, 2, buf, sizeof(buf), rvc_bits); // В rvc_bits будет 40
  if(!res) return 0;

  uint8_t crc = 0;
  for(uint8_t i = 0; i < 4 ; i++) crc ^= buf[i];
  if(crc != buf[4]) return 0; // crc error

  return ((uint32_t)buf[0] << 24)| ((uint32_t)buf[1] << 16) | ((uint32_t)buf[2] << 8) | ((uint32_t)buf[3]);     
 }//_get_uid
} //namespace

/*
 * Детальная инфа:
 * 
 * https://russianblogs.com/article/8815208890/
 * 
 * https://github.com/song940/RFID-RC522/blob/master/RFID.cpp
 * 
 * https://github.com/miguelbalboa/rfid/blob/master/src/MFRC522.cpp
 * 
*/
using namespace RC522;
// == Main plugin function ==
void plgRC522(){
  // Init
  SPI.begin();
  _init();
   
  uint32_t uid;
  core.moveCursor(0, 1);
  core.println(FF(MS_MSG_CARD_PLACE)); 
  
  // Main loop
  while(1){
    while(!_is_card_ready()) delay(READ_DELAY_MS); // wait for card
    uid = _get_uid(); // get card ID
    
    // ID to the screen
    core.moveCursor(0, 1);
    core.print(FF(MS_MSG_CARD_ID));
    core.printLongAsArray(core, uid);
    core.println();
    
    // ID to the serial
    Serial.print(FF(MS_MSG_CARD_ID));
    core.printLongAsArray(Serial, uid);
    Serial.println();   
  }//while  
}//plgRC522
