#include "plgAHT20.h"
#include <Wire.h>
#include "uAHT20.h"

#define SDA_PIN P6 //Yellow-Black
#define SCL_PIN P7 //Gray-Black
#define READ_DELAY_MS 500 // 0.5 seconds between attempt

#define AHT_ADDRESS 0x38 // AHT20 I²C address 

namespace AHT20 { 

} //namespace

using namespace AHT20;
// == Main plugin function ==
void plgAHT20(){
  // Init
  uint8_t need_header = 1;
  Wire.begin();   

  uAHT20_Data aht; // AHT Data
  
  if(!uAHT20_Init(AHT_ADDRESS, aht)) Serial.println(FF(MS_MSG_READ_ERROR));
    
  // Main loop
  while(1){
    core.moveCursor(0, 1);
   
    if(aht.status != 0xFF){ // Read from AHT20
      if(uAHT20_Read(AHT_ADDRESS, aht)){
        core.print(F("T:"));
        core.printValScale(core, aht.temp / 10); 
        core.print(MS_SYM_DEGREE_CODE);
        core.print(F("C H:"));
        core.printValScale(core, aht.hum / 10);
        core.println(F("%"));

        core.printValScale(Serial, aht.temp / 10);
        Serial.print(", ");      
        core.printValScale(Serial, aht.hum / 10);
        Serial.println();                    
      }//if
      else {
        Serial.println(FF(MS_MSG_READ_ERROR));        
        need_header = 1;
      }//if..else
    }//if
    
    if(need_header){
      Serial.print(F("Temperature(°C), Humidity(%), "));
      Serial.print(F("("));
      Serial.print(READ_DELAY_MS);
      Serial.println(FF(MS_MSG_DELAY_END));      
      need_header = 0;
    }//if
    delay(READ_DELAY_MS);    
  }//while  
}//plgAHT20
