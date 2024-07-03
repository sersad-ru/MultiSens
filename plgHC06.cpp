#include "plgHC06.h"
#include "uSST.h"
#include <SoftwareSerial.h>

#define RX_PIN P0
#define TX_PIN P1
#define BT_SERIAL_SPEED 9600
#define TIMEOUT_MS 1000
#define BUF_SIZE 32

namespace HC06 {
   
  // Отправить строку и ждать ответа до таймаута  
  uint8_t _try_command(const SoftwareSerial &ser, char* buf, const char* cmd, const char* line_end=NULL){
    ser.print(cmd);    
    if(line_end) ser.print(line_end);
    while(ser.available()) ser.read();
    return uSST_ReadString(ser, buf, BUF_SIZE, TIMEOUT_MS);
  }//_try_command


  // Функия проверки соединения по COM-порту
  uint8_t _probe(const SoftwareSerial &ser, char* buf, const uint8_t buf_size, const uint32_t timeout_ms){
    for(uint8_t i = 0; i < 3; i++){ // Пробуем 3 раз
      if(_try_command(ser, buf, "AT", "\r\n")){ // Шлем "AT\r\n" и ждем ответа
        if((buf[0] == 'O') && (buf[1] == 'K')) return 1; // Получили ответ
      }//if      
      
      if(_try_command(ser, buf, "AT")){ // Шлем "AT" и ждем ответа
        if((buf[0] == 'O') && (buf[1] == 'K')) return 1; // Получили ответ
      }//if     
    }//for
    return 0; // Нет нужного ответа
  }//_probe


  // Определяем linvor
  uint8_t _check_linvor(const SoftwareSerial &ser, char* buf){
    if(!(_try_command(ser, buf, "AT+VERSION")) && !(_try_command(ser, buf, "AT+VERSION", "\r\n"))) return 0;
    if((buf[0] != 'O') && (buf[1] != 'K')) return 0;
    core.println(buf + 2);
    Serial.println(buf + 2);      
    return 1;
  }//_check_linvor


  // Отправляет команду, отрезает часть ответа и выводит что осталось
  uint8_t _send_cut_print(const SoftwareSerial &ser, char* buf, const char* cmd, const char* cut, const char* pref){
    if(!_try_command(ser, buf, cmd, "\r\n")) return 0;
    char* res = strstr(buf, cut);
    if(!res) return 0;
    res += strlen(cut);
    core.print(pref);
    core.print(res);
    core.print(" ");
    Serial.print(pref);
    Serial.print(res);
    Serial.print(" ");    
    return 1;
  }//_send_and_cut


  // Определяем HC-05/06
  uint8_t _check_hc(const SoftwareSerial &ser, char* buf){
    if(!_send_cut_print(ser, buf, "AT+NAME?", "+NAME:", "")) return 0;
    if(!_send_cut_print(ser, buf, "AT+PSWD?", "+PIN:", "P:")) return 0;
    if(!_send_cut_print(ser, buf, "AT+ADDR?", "+ADDR:", "A:")) return 0;
    if(!_send_cut_print(ser, buf, "AT+VERSION?", "VERSION:", "V:")) return 0;
    return 1;  
  }//_check_hc


  // Определяем JDY-31
  uint8_t _chech_jdy(const SoftwareSerial &ser, char* buf){
    if(!_send_cut_print(ser, buf, "AT+NAME", "+NAME=", "")) return 0;
    if(!_send_cut_print(ser, buf, "AT+PIN", "+PIN=", "P:")) return 0;
    if(!_send_cut_print(ser, buf, "AT+LADDR", "+LADDR=", "A:")) return 0;
    if(!_send_cut_print(ser, buf, "AT+VERSION", "+VERSION=", "V:")) return 0;
  }//_chech_jdy

  
  // Определяем и выводим тип устройства
  void _check_device(const SoftwareSerial &ser, char* buf){
    if(_check_linvor(ser, buf)) return; // Определяем linvor
    if(_chech_jdy(ser, buf)) return; // Определяем JDY-31
    if(_check_hc(ser, buf)) return; // Определяем hc-05/06 
  }//_check_device

}// namespace

/*
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
  uint32_t ser_speed = uSST_FindSpeed(ser, &_probe, buf, BUF_SIZE, TIMEOUT_MS, BT_SERIAL_SPEED);
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
  
  Serial.println(F("\nBluetooth serial log:"));

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
