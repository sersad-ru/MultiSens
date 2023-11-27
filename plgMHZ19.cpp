#include "plgMHZ19.h"
#include <SoftwareSerial.h>

#define RX_PIN P0
#define TX_PIN P1
#define READ_DELAY_MS 3000 // 3 seconds betwen reading

#define READ_ERROR -1 // Packet read error
#define CRC_ERROR  -2 // CRC error

#define START_BYTE 0xff
#define CMD_READ   0x86


// MH-Z19 packet, presetted for CO2 read command 
typedef struct {
  uint8_t startByte  = START_BYTE; // Always 0xff
  uint8_t sensorNum        = 0x01; // Must be 0x01
  uint8_t cmd_co2high  = CMD_READ; // Read CO2 command / High byte of CO2 concentration in response packet
  uint8_t data_high_co2low = 0x00; // Low byte of CO2 concentration in response and high byte in Range/Span/ABC logic command
  uint8_t data_low         = 0x00; // Low byte in Range/Span/ABC logic command
  uint8_t reserved[3]      = {0x00, 0x00, 0x00}; // Reserved bytes
  uint8_t crc              = 0x79; // CRC. Precalculated for this packet
} mzhPacket;


const char MHZ19_CRC_ERROR[] PROGMEM = "CRC Error";

int16_t _read(SoftwareSerial &ser){
  mzhPacket pkt; // Пакет
  uint8_t * pkt_ptr = (uint8_t *)&pkt; // Указатель на область памяти пакета
  
  ser.write(pkt_ptr, sizeof(pkt)); // Отправляем запрос
  ser.available(); //Тут какая-то хрень. без этого обращение из порта читается неверное количество даных

  ser.readBytes(pkt_ptr, sizeof(pkt)); // Читаем ответ
  while(ser.available()){ ser.read(); } // Подчищаем буфер
  
  if ((pkt.startByte != START_BYTE) | (pkt.sensorNum != CMD_READ)) return READ_ERROR; // Ошибка чтения пакета
  
  // CRC
  uint8_t crc = 0;
  for(uint8_t i = 0; i < sizeof(pkt) - 1; i++) crc+= pkt_ptr[i]; //Складываем все, кроме последнего (crc) байта
  
  if(pkt.crc != (0xff - crc)) return CRC_ERROR; //По даташиту надо к (0xff - crc) еще прибавить 1, но тогда не сходится :(
  return (pkt.cmd_co2high) << 8 | pkt.data_high_co2low;
}//_read


// == Main plugin function ==
void plgMHZ19(){
  // Init
  SoftwareSerial ser(RX_PIN, TX_PIN);
  ser.begin(9600);
 
  int16_t ppm;
  uint8_t need_header = 1; 
  
  // Main loop
  while(1){
    core.moveCursor(0, 1);
    
    if(need_header){ // Need to print header for serial plotter
      Serial.print(F("CO2(PPM), ("));  
      Serial.print(READ_DELAY_MS);
      Serial.println(FF(MS_MSG_DELAY_END));
      need_header = 0;
    }//if
    
    ppm = _read(ser); // Read CO2 concentration from the sensor
    switch(ppm){
      case READ_ERROR: 
        core.println(FF(MS_MSG_READ_ERROR));
        Serial.println(FF(MS_MSG_READ_ERROR));
        need_header = 1; // //Next time print header again
        break;

      case CRC_ERROR:  
        core.println(FF(MHZ19_CRC_ERROR));
        Serial.println(FF(MHZ19_CRC_ERROR));
        need_header = 1; // //Next time print header again
        break;

      default: // Print PPM value
        core.print(F("CO2: "));
        core.print(ppm);
        core.println(F(" ppm"));

        Serial.println(ppm);
        break;
    }//switch   

    delay(READ_DELAY_MS);
  }//while  
}//plgMHZ19
