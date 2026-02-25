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

#include "plgRDM6300.h"
#include "mscardmsg.h"
#include <SoftwareSerial.h>

#define RX_PIN P0
#define TX_PIN P1
#define READ_DELAY_MS 500 // 0.5 seconds between attempt

namespace RDM6300 { 
 
// RDM6300 packet
typedef struct {
  uint8_t  head;   // Pkt Header (0x02 - tag incoming, 0x03 - tag has been fully transmitted)
  uint8_t  ver[2]; // Verson
  uint8_t  id[8];  // Card ID
  uint8_t  crc[2]; // CRC
  uint8_t  tail;   // Pkt Tail
} rdmPacket;


uint32_t _str2long(const char *buf, uint8_t size){
  uint32_t res = 0;
  for(uint8_t i = 0; i < size; i++){
    if((buf[i] < '0') || (buf[i] > 'F')) return 0;
    if((buf[i] > '9') && (buf[i] < 'A')) return 0;   
    uint32_t val = ((buf[i] > '9') ? ((buf[i] - 'A') + 10) : (buf[i] - '0'));    
    res |= val << ((size - 1 - i) * 4);
  }//for
  return res;
}//_str2long

  
uint32_t _read_id(SoftwareSerial &ser){
  rdmPacket pkt;  
  
  if(!ser.available()) return 0;
  
  ser.readBytes((uint8_t*)&pkt, sizeof(pkt));
  while(ser.available()) ser.read(); // Чистим буфер

  // CRC ver + id шагом по два байта. Результат сравнивать с _str2long((char*)&pkt.crc, 2)
  uint32_t crc = _str2long((char*)&pkt.ver, 2);
  for(uint8_t i = 0; i < 4; i++) crc ^= _str2long((char*)&pkt.id[i * 2], 2);

  if(crc != _str2long((char*)&pkt.crc, 2)) return 0; 
  
  return _str2long((char*)&pkt.id, 8);
}//_read

} //namespace


using namespace RDM6300;

// == Main plugin function ==
void plgRDM6300(){
  // Init
  SoftwareSerial ser(RX_PIN, TX_PIN);
  ser.begin(9600);

  uint32_t id;
  core.moveCursor(0, 1);
  core.println(FF(MS_MSG_CARD_PLACE)); 

  // Main loop
  while(1){       
    // Tryint to read ID   
    while(!(id = _read_id(ser))) delay(READ_DELAY_MS);

    // ID to the screen
    core.moveCursor(0, 1);
    core.print(FF(MS_MSG_CARD_ID));
    core.printLongAsArray(core, id);
    core.println();

    // ID to the serial
    Serial.print(FF(MS_MSG_CARD_ID));
    core.printLongAsArray(Serial, id);
    Serial.println();
  }//while  
}//plgRDM6300
