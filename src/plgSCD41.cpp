/*
Arduino Multi Senstor Tester
(c)2023-2026 by Sergey Sadovnikov (sersad@gmail.com)

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

#include "plgSCD41.h"
#include <Wire.h>


#define SDA_PIN P6 //Yellow-Black
#define SCL_PIN P7 //Gray-Black

#define READ_DELAY_MS 5000 // 5 seconds between attempt

#define SCD41_ADDRESS          0x62 // SCD41 I²C address 
#define CMD_STOP_PERIODIC    0x3f86 // Stop pereodic measurement command
#define CMD_START_PERIODIC   0x21B1 // Start pereodic measurement command
#define CMD_READ_MEASUREMENT 0xEC05 // Read values command

#define READ_ERROR -1 // I²C read error
#define CRC_ERROR  -2 // CRC error
namespace SCD41 { 

const char SCD41_CRC_ERROR[] PROGMEM = "CRC Error";

struct Measurement {
  int16_t co2;
  int32_t temperature_x10; // e.g., 254 represents 25.4 °C
  int32_t humidity_x10;    // e.g., 505 represents 50.5 %
};

// Sensirion CRC-8 calculation (Polynomial: 0x31, Init: 0xFF)
uint8_t _calcCRC8(uint8_t msb, uint8_t lsb) {
  uint8_t crc = 0xFF;
  uint8_t data[2] = {msb, lsb};
  for (uint8_t i = 0; i < 2; i++) {
    crc ^= data[i];
    for (uint8_t bit = 8; bit > 0; --bit) 
      if (crc & 0x80) crc = (crc << 1) ^ 0x31;
       else crc = (crc << 1);          
  }//for
  return crc;
}// _calcCRC


int8_t _read(Measurement &m) {
  // 1. Send Read Measurement command (16-bit)
  Wire.beginTransmission(SCD41_ADDRESS);
  Wire.write((uint8_t)(CMD_READ_MEASUREMENT >> 8));
  Wire.write((uint8_t)(CMD_READ_MEASUREMENT & 0xFF));

  if (Wire.endTransmission() != 0) return READ_ERROR;
  
  // 2. SCD41 needs ~50ms to process the read command
  delay(50);

  // 3. Request 9 bytes (3 values * 3 bytes (2 data + 1 CRC))
  Wire.requestFrom(SCD41_ADDRESS, (uint8_t)9);
  if (Wire.available() < 9) return READ_ERROR;
  
  // 4. Read CO2
  uint8_t co2_msb = Wire.read();
  uint8_t co2_lsb = Wire.read();
  uint8_t co2_crc = Wire.read();
  if (_calcCRC8(co2_msb, co2_lsb) != co2_crc) return CRC_ERROR;
  m.co2 = (co2_msb << 8) | co2_lsb;

  // 5. Read Temperature
  uint8_t t_msb = Wire.read();
  uint8_t t_lsb = Wire.read();
  uint8_t t_crc = Wire.read();
  if (_calcCRC8(t_msb, t_lsb) != t_crc) return CRC_ERROR;
  uint16_t t_raw = (t_msb << 8) | t_lsb;
  
  // Fixed-point math: T_x10 = -450 + (1750 * t_raw) / 65535
  // 1750 * 65535 = 114,686,250, which safely fits in int32_t (max 2,147,483,647)
  m.temperature_x10 = -450L + (1750L * (int32_t)t_raw) / 65535L;

  // 6. Read Humidity
  uint8_t h_msb = Wire.read();
  uint8_t h_lsb = Wire.read();
  uint8_t h_crc = Wire.read();
  if (_calcCRC8(h_msb, h_lsb) != h_crc) return CRC_ERROR;
  uint16_t h_raw = (h_msb << 8) | h_lsb;
  
  // Fixed-point math: RH_x10 = (1000 * h_raw) / 65535
  // 1000 * 65535 = 65,535,000, which safely fits in int32_t
  m.humidity_x10 = (1000L * (int32_t)h_raw) / 65535L;
  return 0; // Success
}// _read

} // namespace

using namespace SCD41;

// == Main plugin function ==
void plgSCD41(){
  // Init
  Wire.begin(); 

  Measurement m;
  uint8_t need_header = 1;
  uint32_t start_time = millis();
  bool is_ready = false;

  // Stop periodic measurement mode
  Wire.beginTransmission(SCD41_ADDRESS);
  Wire.write((uint8_t)(CMD_STOP_PERIODIC >> 8));
  Wire.write((uint8_t)(CMD_STOP_PERIODIC & 0xFF));
  Wire.endTransmission();  

  delay(500);

  // Start periodic measurement mode
  Wire.beginTransmission(SCD41_ADDRESS);
  Wire.write((uint8_t)(CMD_START_PERIODIC >> 8));
  Wire.write((uint8_t)(CMD_START_PERIODIC & 0xFF));
  Wire.endTransmission();  

  // Main loop
  while(1){
    core.moveCursor(0, 1);
    
    if (need_header) {
      Serial.print(F("CO2(ppm), Temp(C), Hum(%), ("));  
      Serial.print(READ_DELAY_MS);
      Serial.println(F(" ms delay)"));
      need_header = 0;
    }//if
    
    // Wait for the first valid data (SCD41 requires ~5 seconds after start command)
    if (millis() - start_time < 5000) {
      core.println(F("  Wait 5 sec..."));
      Serial.println(F("Waiting for data..."));
    } else {
      is_ready = true;
    }//if..else

    if (is_ready) {
      int8_t res = _read(m);
      switch (res) {
        case READ_ERROR: 
          core.println(FF(MS_MSG_READ_ERROR));
          Serial.println(FF(MS_MSG_READ_ERROR));
          need_header = 1; // Print header again on next cycle
          break;

        case CRC_ERROR:  
          core.println(FF(SCD41_CRC_ERROR));
          Serial.println(FF(SCD41_CRC_ERROR));
          need_header = 1; // Print header again on next cycle
          break;

        default: // Print valid values using fixed-point function from mscore
          // Округление до целых (с учетом знака для температуры)
          int32_t t_rounded = (m.temperature_x10 + (m.temperature_x10 >= 0 ? 5 : -5)) / 10;
          int32_t h_rounded = (m.humidity_x10 + 5) / 10; 

          core.print(m.co2);
          core.print(F("ppm, "));
          core.print(t_rounded);
          core.print(MS_SYM_DEGREE_CODE);
          core.print(F(", "));
          core.print(h_rounded);
          core.println(F("%"));

          Serial.print(m.co2);
          Serial.print(", ");
          core.printValScale(Serial, m.temperature_x10); // Вывод в Serial
          Serial.print(", ");
          core.printValScale(Serial, m.humidity_x10);    // Вывод в Serial
          Serial.println();
          break;
      }// switch
    }//if

    delay(READ_DELAY_MS);
  }//while  
}//plgSCD41
