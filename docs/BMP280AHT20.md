# MultiSens

Arduino Multi Sensor Tester

## BMP280 and AHT20 Plugin
BMP280+AHT20 plugin receuve temperature, humidity and pressure air from I²C module with 
BMP280 and AHT20 sensors.

Results are displayed on the device screen and sends to serial in human-readable and 
Arduino `SerialPlotter` compartible format.

`BMP280` used for air pressure only and `AHT20` for temperature and humidity.

* You can specify the delay between sensors calls using `READ_DELAY_MS` 
  in [plgBMP280AHT20.cpp](/plgBMP280AHT20.cpp)

* BMP280 I²C address in `BMP_ADDRESS` in [plgBMP280AHT20.cpp](/plgBMP280AHT20.cpp)
* AHT20 I²C address in `AHT_ADDRESS` in [plgBMP280AHT20.cpp](/plgBMP280AHT20.cpp)

### Connection


[Back to Home](/#supported-devices)

