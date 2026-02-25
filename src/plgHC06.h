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

#pragma once
#include <Arduino.h>
#include "mscore.h" 

/*
* The HC06 plugin receives information from the HC-05 or HC-06 Bluetooth module.
* Connect the module and run the plugin. 
* The module type will be printed on the screen. 
* Attention! If you got "No device found" message with HC-05 module, you should press and hold the `KEY` button on the module before plugin starts. You can unhold the button when module info will be displayed.
* The module LED will blink continuosly. 
* Connect to the bluetooth module using Bluetooth Terminal(https://f-droid.org/ru/packages/ru.sash0k.bluetooth_terminal/) or similar software. 
* The default PIN is 1234
* When you enter any text in the terminal, it will be displayed on the device screen, transmitted to the serial port and sends back via bluetooth.
*
* Connection:
* GND - GND - black
* VCC - +5V - red
* TXD - P0 - green  
* RXD - P1 - blue
* 
*/

/* The main plugin function declaration */
void plgHC06();
