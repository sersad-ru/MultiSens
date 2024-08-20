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
* The ModBus Scan plugin searches for the first five ModBus RTU devices on the ModBus bus.
*
* Results are displayed on the device screen and sends to the `Serial` in human-readable format. 
* 
* Due to some limitations of `SoftwareSerial` library this plugin can search for devices with 
* `8N1` serial port configuration only.
*
* Connect the module to the MultiSens and to the bus and run the plugin.
* The MultiSens will try to find the module. 
* Current progress (number of founded devices, search percents and estimated time to the end) 
*  will be displayed on the screen and send to the Serial.
* You can interrupt the search process any time by pressing the `SELECT` button.
* When the search process will be finished or interrupted the list of founded devices will be displayed. 
* You can scroll it by pressing the `UP` and `DOWN` buttons.
* The maximum number of devices to find is stored in `SCAN_MAX` constant (see [plgModBusSearch.cpp]).
* The device reply timeout (in milliseconds) is stored in `TIMEOUT_MS` constant (see [plgModBusSearch.cpp]).
*
* 
* Connection:
* GND - GND - black
* VCC - +5V - red
* DI  - P0  - green (TX)
* DE  - P2  - yellow
* RE  - P2 -  yellow
* RO  - P1  - blue (RX)
*/

void plgModBusScan();
