#pragma once
#include <Arduino.h>
#include "mscore.h"
/*
* The REncoder plugin receives signals from the rotary encoder and displays the rotating direction and the button status. 
* When encoder rotated clockwise (CW) it increase the counter. 
* In other direction (CCW) the counter decrease. 
* If the button is pressed, the counter will be resetted to zero.
*
* Don't forget to add pull-up resistors if you are using a standalone encoder (not the preassembled module). 
* 
* Results are displayed on the device screen and sends to the serial in human readable format. 
* 
* 
* Connection:
* GND - GND - black
* VCC - +5V - red
* Key - P0  - green 
* S1  - P1  - blue
* S2  - P2  - yellow 
*
*
*/
/*
struct{
    
} plgREncoderCfg;
*/
void plgREncoder();
