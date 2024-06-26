#include "plgHC06.h"
#include "uSST.h"
#include <SoftwareSerial.h>

#define RX_PIN P0
#define TX_PIN P1
#define TIMEOUT_MS 1000
#define BUF_SIZE 32

namespace HC06 {
  
  // Отправить строку и ждать ответа до таймаута  
  uint8_t _try_command(const SoftwareSerial &ser, char* buf, const __FlashStringHelper* cmd, const char* line_end=NULL){
    ser.print(cmd);    
    if(line_end) ser.print(line_end);
    return uSST_ReadString(ser, buf, BUF_SIZE, TIMEOUT_MS);
  }//_try_command


  // Функия проверки соединения по COM-порту
  uint8_t _probe(const SoftwareSerial &ser, char* buf, const uint8_t buf_size, const uint32_t timeout_ms){
    for(uint8_t i = 0; i < 3; i++){ // Пробуем 3 раз
      if(_try_command(ser, buf, F("AT"))){ // Шлем "AT" и ждем ответа
        if((buf[0] == 'O') && (buf[1] == 'K')) return 1; // Получили ответ
      }//if
      
      if(_try_command(ser, buf, F("AT"), "\r\n")){ // Шлем "AT\r\n" и ждем ответа
        if((buf[0] == 'O') && (buf[1] == 'K')) return 1; // Получили ответ
      }//if      
    }//for
    return 0; // Нет нужного ответа
  }//_probe


  // Определяем и выводим тип устройства
  void _check_device(const SoftwareSerial &ser, char* buf){
    if(_try_command(ser, buf, F("AT+PSWD?"), "\r\n")){ 
      // Это HC-05
      core.print("05:");
      Serial.print("05:");      
      
      // Выводим PIN
      core.print(buf);
      Serial.print(buf);      

      // Получаем MAC
      if(_try_command(ser, buf, F("AT+ADDR?"), "\r\n")){
        core.print(':');
        Serial.print(':');              
        
        core.print(buf);
        Serial.print(buf);              
      }//if
      core.println();
      Serial.println();            
      return; // Выходим      
    }//if

    // Смотрим дальше
    if(_try_command(ser, buf, F("AT+VERSION"))){ 
      // Это HC-06
      core.print("06:");
      Serial.print("06:");      
      core.println(buf);
      Serial.println(buf);      
      return;      
    }//if

    // Пробуем с возвратом каретки
    if(_try_command(ser, buf, F("AT+VERSION"), "\r\n")){ 
      // Это HC-06
      core.print("06:");
      Serial.print("06:");      
      core.println(buf);
      Serial.println(buf);      
      return;      
    }//if

    // Ничего не вышло
    core.println(FF(uSST_NO_DEV_MSG));
    Serial.println(FF(uSST_NO_DEV_MSG));      
  }//_check_device

}// namespace

/*
 * Протестировать на HC-05
 * 
 * На телефон:
 * https://f-droid.org/ru/packages/ru.sash0k.bluetooth_terminal/
 * 
 * В ubuntu:
 *   https://amriunix.com/posts/hc-06-bluetooth-in-ubuntu/
 *   https://robotlife.ru/podkluchnie_po_bluetooth_v_ubuntu/
 *   hcitool dev - список своих устройств
 *   hcitool scan - поиск устройства
 *   bluetoothctl - paring (https://www.baeldung.com/linux/bluetooth-via-terminal)
 *   https://devicetests.com/fixing-bluetooth-connection-issues-ubuntu
 *      power on
 *      agent on
 *      scan on
 *      pair <mac>
 *   sudo rfcomm connect 0 <mac> 1 - создаем порт  
 *   sudo apt-get install cutecom - gui терминал. Им коннектимся на /dev/rfcomm0
 * 
*/
using namespace HC06;

// == Main plugin function ==
void plgHC06(){
  char buf[BUF_SIZE];
  
  // Init
  SoftwareSerial ser(RX_PIN, TX_PIN);

  // Dispaly init
  core.moveCursor(0, 1);
  core.println(FF(uSST_CONNECTING_MSG));
  Serial.println(FF(uSST_CONNECTING_MSG));

  // Ищем скорость ком-порта
  uint32_t ser_speed = uSST_FindSpeed(ser, &_probe, buf, BUF_SIZE, TIMEOUT_MS, 9600);
  if(!ser_speed){ // Не нашли устройство
    core.moveCursor(0, 1);
    core.println(FF(uSST_NO_DEV_MSG));
    Serial.println(FF(uSST_NO_DEV_MSG));
    while(1);
  }//if

  core.moveCursor(0, 1);
  core.print(FF(uSST_FOUND_MSG));
  core.println(ser_speed);

  Serial.print(FF(uSST_FOUND_MSG));
  Serial.print(ser_speed);
  Serial.println(F("bps"));
  ser.begin(ser_speed);
  
  core.moveCursor(0, 1);
  _check_device(ser, buf);

  Serial.println(F("Bluetooth serial log:"));

  // Main loop
  while(1){
    if(uSST_ReadString(ser, buf, BUF_SIZE, TIMEOUT_MS)){
      core.moveCursor(0, 1);
      core.println(buf);
      Serial.println(buf);
      ser.println(buf);
    }//if
  }//while
}//plgHC06
