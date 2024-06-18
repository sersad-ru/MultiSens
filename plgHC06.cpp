#include "plgHC06.h"
#include <SoftwareSerial.h>

#define RX_PIN P0
#define TX_PIN P1
#define TIMEOUT_MS 550
#define BUF_SIZE 32

namespace HC06 {
  const char NO_DEV_MSG[] PROGMEM = "No device found";  
  
  // Буфер для чтения строк
  char _buf[BUF_SIZE];

  // Замеяет в буфера первый \r или \n на 0
  void _trim(){
    for(uint8_t i = 0; i < (BUF_SIZE - 1); i++)
      if((_buf[i] == 0x0D) || (_buf[i] == 0x0A)){
        _buf[i] = 0;
        return;
      }//if
  }//_trim
  
  // Читать строку в буфер до таймаута
  uint8_t _read_string(SoftwareSerial &ser){
    uint8_t i = 0;
    uint32_t t_start = millis();
    while((millis() - t_start) < TIMEOUT_MS){
      while(ser.available()){
        _buf[i++] = ser.read();
        if(i > BUF_SIZE - 1) return 1;
      }//while
    }//while
    _trim();
    return i;  
  }//_read_string


  // Отправить строку и ждать ответа до таймаута  
  uint8_t _try_command(SoftwareSerial &ser, const __FlashStringHelper* cmd, const char* line_end=NULL){
    ser.print(cmd);    
    if(line_end) ser.print(line_end);
    return _read_string(ser);
  }//_try_command


  // Определяем и выводим тип устройства
  void _check_device(SoftwareSerial &ser){
    if(_try_command(ser, F("AT+PSWD?"), "\n\r")){ 
      // Это HC-05
      core.print("05:");
      Serial.print("05:");      
      
      // Выводим PIN
      core.print(_buf);
      Serial.print(_buf);      

      // Получаем MAC
      if(_try_command(ser, F("AT+ADDR?"), "\n\r")){
        core.print(':');
        Serial.print(':');              
        
        core.print(_buf);
        Serial.print(_buf);              
      }//if
      core.println();
      Serial.println();            
      return; // Выходим      
    }//if

    // Смотрим дальше
    if(_try_command(ser, F("AT+VERSION"))){ 
      // Это HC-06
      core.print("06:");
      Serial.print("06:");      
      core.println(_buf);
      Serial.println(_buf);      
      return;      
    }//if

    // Пробуем с возвратом каретки
    if(_try_command(ser, F("AT+VERSION"), "\n\r")){ 
      // Это HC-06
      core.print("06:");
      Serial.print("06:");      
      core.println(_buf);
      Serial.println(_buf);      
      return;      
    }//if

    // Ничего не вышло
    core.println(FF(NO_DEV_MSG));
    Serial.println(FF(NO_DEV_MSG));      
  }//_check_device
}// namespace

/*
 * Протестировать на HC-05
 * 
 * https://f-droid.org/ru/packages/ru.sash0k.bluetooth_terminal/
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

  // Init
  SoftwareSerial ser(RX_PIN, TX_PIN);
  ser.begin(9600);
  delay(500);

  // Dispaly init
  core.moveCursor(0, 1);
  _check_device(ser);
/*  
  core.moveCursor(0, 1); // First symbol of second line

  // Пытаемся определить тип устройства
  if(_try_command(ser, F("AT+VERSION"))){
  //if(_try_command(ser, F("AT+ADDR?"), "\r\n")){
  //if(_try_command(ser, F("AT+PSWD?"))){
    core.println(_buf);
    Serial.println(_buf);
  }//if
  else Serial.println("Timeout");  
*/

  Serial.println(F("Bluetooth serial log:"));

  // Main loop
  while(1){
    if(_read_string(ser)){
      core.moveCursor(0, 1);
      core.println(_buf);
      Serial.println(_buf);
    }//if
  }//while
}//plgHC06
