#include "mspinmap.h"
#include <Arduino.h>

uint8_t _ms_getP(uint8_t aPin){
  uint8_t res = 0; 
  for(; res < 8; res++) 
    if(P(res) == aPin) break; 
  return res;
}
