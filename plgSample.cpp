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
