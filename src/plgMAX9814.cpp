#include "HardwareSerial.h"
/*
Arduino Multi Senstor Tester
(c)2023-2025 by Sergey Sadovnikov (sersad@gmail.com)

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

#include "plgMAX9814.h"
#include "uFD.h"

#define MIC_PIN P5 //Orange
#define DISPLAY_DELAY_MS 500

#define VOL_MIN 100
#define VOL_MAX 600
#define SYM_MAX 10

namespace MAX9814 { 
} //namespace

using namespace MAX9814;
// == Main plugin function ==
void plgMAX9814(){
  // Init
  uint16_t vol = 0;
  uint32_t last_display;

  TIMSK1 &= ~(1 << TOIE1); // Отключаем таймер 1, на котором висит опрос кнопок.

  // Main loop
  while(1){
    uint16_t freq = uFD_getFreq(MIC_PIN, vol);
    if(freq <= uFD_ERR_MAX_ERROR) continue;

    if((millis() - last_display) < DISPLAY_DELAY_MS) continue;
    last_display = millis();

    core.moveCursor(0, 1);

    uint8_t sym_cnt = map(vol, VOL_MIN, VOL_MAX, 0, SYM_MAX);
    for(uint8_t i = 0; i < sym_cnt; i++) core.print(MS_SYM_PROGRESS_CODE);
    for(uint8_t i = 0; i < (SYM_MAX - sym_cnt); i++) core.print(' ');

    core.print(core.rAlign(freq, 4));
    core.println(F("Hz"));

    Serial.print("Volume:");
    Serial.print(vol);
    Serial.print(", Frequency(Hz):");
    Serial.println(freq);
  }//while  
}//plgMAX9814
