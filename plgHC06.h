#pragma once
#include <Arduino.h>
#include "mscore.h" 

/*
* The HC06 plugin receives information from the HC-05 or HC-06 Bluetooth module.
* Connect the module and run the plugin. 
* The module type will be printed on the screen. 
* The module LED will blink continuosly. 
* Connect to the bluetooth module using Bluetooth Terminal(https://f-droid.org/ru/packages/ru.sash0k.bluetooth_terminal/) or similar software. 
* When you enter any text in the terminal, it will be displayed on the device screen and transmitted to the serial port.
*
* Connection:
* GND  - GND - black
* VCC  - +5V - red
* TXD - P0 - green  
* RXD - P1 - blue
* 
*/

/* The main plugin function declaration */
void plgHC06();
