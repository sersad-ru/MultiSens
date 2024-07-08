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

#include "plgSample.h"

#define INPUT_PIN P0
#define READ_DELAY_MS 500 // 0.5 seconds between attempt

// == Main plugin function ==
void plgSample(){

  // Init    
  pinMode(INPUT_PIN, INPUT);
  
  // Dispaly init
  core.moveCursor(0, 1); // First symbol of second line

  core.print(MS_SYM_SELECT_CODE); // The SELECT button symbol
  core.println(F("-to start/stop"));

  uint8_t can_read = false; // Read enable flag
  uint8_t value;

  // Main loop
  while(1){
    // Process user input    
    switch (core.getButton()) {
      case SELECT: // React on SELECT button
        can_read = !can_read; // enable/disable reading
      break; 
    
      default: break;
    }//switch

    if(!can_read) continue; // reading is disabled

    // Read results
    value = digitalRead(INPUT_PIN);

    // Display results on the screen
    core.moveCursor(0, 1); // First symbol of second line
    core.print(F("Value: "));
    core.println(value);

    // Send results to the Serial
    Serial.print(F("Value: "));
    Serial.println(value);

    delay(READ_DELAY_MS); // Delay between attemps
  }//while
}//plgSample
