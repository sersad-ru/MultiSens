# MultiSens

Arduino Multi Sensor Tester

<p align="center"><img src="docs/MultiSens.jpg"/></p>


## Table of contents
* [Overview](#overview)
* [Supporded devices](#supported-devices)
* [Hardware](#hardware)
* [Software](#software)
* [Adding plugins](#adding-plugins)


## Overview
Sometimes something goes wrong in your project. Maybe you have an error in your schematic.
Maybe there are some bugs in your code. Or your new sensor is damaged. 

This simple tool can helps you to solve one of this problem: **the sensor**.
You can easily connect a sensor to the Multisens, select compartible plugin and check 
if the sensor works properly or not.


## Supported Devices
Currently MuliSens supports following list of devices:

|Plugin|Description|
|---|---|
|[FindPin](docs/FindPin.md)|Displays the `msPin` number of selected wire|
|[I²CScan](docs/I2CScan.md)|Searches for I²C devices|
|[DigitalAnalog](docs/DigitalAnalog.md)|Reads one digital and one analog pin|
|[DigitalRead](docs/DigitalRead.md)|Reads digital pins|
|[AnalogRead](docs/AnalogRead.md)|Reads analog pins|
|[PWM](docs/PWM.md)|Creates the PWM signal on pins|
|[DS18B20](docs/DS18B20.md)|Temperature sensor|
|[AM2320/DHT22](docs/AM2302.md)|Temperature and Humidity sensor|
|[AHT20](docs/AHT20.md)|Temperature and Humidity sensor|
|[BME280](docs/BME280.md)|Temperature, Humidity and Pressure sensor|
|[BMP280+AHT20](docs/BMP280AHT20.md)|Temperature, Humidity and Pressure module| 
|[HTU21D](docs/HTU21D.md)|Temperature and Humidity sensor|
|[iButton](docs/IButton.md)|DS19xx Serial Number reader|
|[MH-Z19b](docs/MH-Z19.md)|CO₂ concentration sensor|
|[QRE1113](docs/QRE1113.md)|Distance sensor|
|[HC-SR04](docs/HC-SR04.md)|Distance sensor|
|[RC522](docs/RC522.md)|RFID 13.56 MHz Card reader|
|[RDM6300](docs/RDM6300.md)|RFID 125 kHz Card reader|


## Hardware
<p align="center"><img src="docs/Minimal.jpg"/></p>

The hardware is pretty simple. In minimal configuration you need only two modules: `Arduino Uno` 
and `Adruino LCD Keypad Shield`. Just solder 11 wires to Arduino according following table and connect
LCD Keypad Shield. That's all.


|Arduino Pin|MultiSens Pin|Color|Function|
|:---:|:---:|:---|:---|
|GND|GND|Black|Ground|
|5v|+5V|Red|+5V power source|
|3v3|+3.3V|White|+3.3V power source|
|D2|P0|Green|Digital I/O, INT0|
|D3|P1|Blue|Digital I/O, INT1, PWM|
|D11|P2|Yellow|Digital I/O, MOSI, PWM|
|D12|P3|Violet|Digital I/O, MISO|
|D13|P4|Brown|Digital I/O, SCK|
|D17[A3]|P5|Orange|Analog In|
|D18[A4]|P6|Yellow-Black|Analog In, SDA|
|D19[A5]|P7|Gray-Black|Analog In, SCL|


#### The Case
You can print a case with mini breadboard, cables compartment and power bank holder.
А power consumption was too low for my power bank. 
So i decided to add a small flashlight (LED with a resistor and switch) to prevent my power bank 
from automatically turning off in a few seconds.

To use this version of the case you need an Arduino Uno with a `USB Type-C` connector 
(a regular `USB-B` connector doesn't fit this enclosure). And you should unsolder the 
arduino power jack socket.

<p align="center"><img src="docs/CaseModel.jpg"/></p>
Link to the thingverse.

## Software

#### Install

* Create MultiSens folder in your Arduino sketch directory.
* Open it
* Clone this repository
* Run Arduino environment, compile the code and upload it to the board.

```bash
mkdir MultiSens
cd MultiSens
git clone https://github.com/sersad-ru/MultiSens
```

How to use.

<p align="center"><img src="docs/Buttons.jpg"/></p>

How to add/remove plugins.

## Adding plugins
How to write your own plugin.
