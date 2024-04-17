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
