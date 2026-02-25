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

#include "plgIButton.h"
#include "uOneWire.h"

#define WORK_PIN P0

namespace iButton {

void _print_family(Print &p, const uint8_t code){
  p.print(F("DS19"));
  switch(code){
    case 0x01: p.print(F("90A (0x")); break;
    case 0x02: p.print(F("91 (0x")); break;
    case 0x04: p.print(F("94 (0x")); break;
    case 0x06: p.print(F("93 (0x")); break;      
    case 0x08: p.print(F("92 (0x")); break;
    case 0x09: p.print(F("82 (0x")); break;                
    case 0x0a: p.print(F("95 (0x")); break;
    case 0x0b: p.print(F("85 (0x")); break;
    case 0x0c: p.print(F("96 (0x")); break;
    case 0x0f: p.print(F("86 (0x")); break;
    case 0x10: p.print(F("20 (0x")); break;
    default:   p.print(F("?? (0x")); break;
  }//switch
  uOW_hex2dig(p, code);
  p.print(")");
}//_print_family

} // namespace

using namespace iButton;

// == Main plugin function ==
void plgIButton(){
  // Init 
  uOW_ROM rom;
  uOW_Errors err;

  core.moveCursor(0, 1);
  core.print(F("Press "));
  core.print(MS_SYM_SELECT_CODE);
  core.print(F(" to start."));

  
  // Main loop
  while(1){
    if(core.wait4Button() != SELECT) continue; // Waitin for <SELECT> to start
    
    core.moveCursor(0, 1);
    
    if(!uOW_readROM(WORK_PIN, rom, err)){
      uOW_printError(core, err, rom.crc); // If it was the CRC-error we'll print CRC from device to the screen...
      core.println();
      uOW_printError(Serial, err, rom.crc); //.. and to the serial too
      Serial.println();
      continue;
    }//if

   // Serial number and family code to the screen
   //uOW_printSerial(core, rom.serialNumber);
   core.printHexArray(core, rom.serialNumber, arraySize(rom.serialNumber));
   core.print(" ");
   _print_family(core, rom.familyCode);

   // Serial number and family code to the serial
   //uOW_printSerial(Serial, rom.serialNumber);
   core.printHexArray(Serial, rom.serialNumber, arraySize(rom.serialNumber));
   Serial.print(" ");
   _print_family(Serial, rom.familyCode);
   Serial.println();
  }//while
}//
