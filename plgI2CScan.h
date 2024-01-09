#pragma once
#include <Arduino.h>
#include "mscore.h"

/*
* The I2C Scanner plugin searches for first I2C_MAX_DEVICES (see plgI2CScan.cpp) devices on I2C bus.
* Connection:
* GND - GND - black
* VCC - +5V - red
* SDA - P6 - yellow-black
* SCL - P7 - gray-black
*/

void plgI2CScan();
