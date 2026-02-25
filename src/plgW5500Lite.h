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
* The W5500Lite plugin requests the network settings (IP, Netmask and Gateway) from a DHCP server.
* Results are printed on screen and sends to serial in human readable format.
* Use the UP and DOWN buttons to display IP-adress, Netmask, Gateway and DHCP-server address.
* Use the SELECT button to start infinity ping requests to the gateway.
* Use the SELECT button to hold ping process end start it again.
* 
* Connection:
* GND  - GND   - black
* VCC  - +3.3V - white
* CS   - P0    - green 
* MOSI - P2    - yellow
* MISO - P3    - violet
* SCK  - P4    - brown
*
*/
/*
struct{
  uint8_t data;  
} plgW5500LiteCfg;
*/
void plgW5500Lite();
