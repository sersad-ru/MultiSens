# MultiSens

The Arduino Multi Sensor Tester

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

|Plugin|Description|Activated|
|---|---|:---:|
|[FindPin](docs/FindPin.md)|Displays the `msPin` number of selected wire|Yes|
|[I²CScan](docs/I2CScan.md)|Searches for I²C devices|Yes|
|[DigitalAnalog](docs/DigitalAnalog.md)|Reads one digital and one analog pin|Yes|
|[DigitalRead](docs/DigitalRead.md)|Reads digital pins|Yes|
|[AnalogRead](docs/AnalogRead.md)|Reads analog pins|Yes|
|[PWM](docs/PWM.md)|Creates the PWM signal on pins|Yes|
|[DS18B20](docs/DS18B20.md)|Temperature sensor|Yes|
|[AM2320/DHT22](docs/AM2302.md)|Temperature and Humidity sensor|Yes|
|[AHT20](docs/AHT20.md)|Temperature and Humidity sensor|Yes|
|[BME280](docs/BME280.md)|Temperature, Humidity and Pressure sensor|Yes|
|[BMP280+AHT20](docs/BMP280AHT20.md)|Temperature, Humidity and Pressure module|Yes|
|[HTU21D](docs/HTU21D.md)|Temperature and Humidity sensor|Yes|
|[iButton](docs/IButton.md)|DS19xx Serial Number reader|Yes|
|[MH-Z19b](docs/MH-Z19.md)|CO₂ concentration sensor|Yes|
|[QRE1113](docs/QRE1113.md)|Infrared distance sensor|Yes|
|[HC-SR04](docs/HC-SR04.md)|Ultrasonic distance sensor|Yes|
|[RC522](docs/RC522.md)|RFID 13.56 MHz Card reader|Yes|
|[RDM6300](docs/RDM6300.md)|RFID 125 kHz Card reader|Yes|
|[ADXL345](docs/ADXL345.md)|3-axes accelerometer|Yes|
|[MPU6050](docs/MPU6050.md)|3-axes accelerometer and gyroscope|Yes|
|[WS2812](docs/WS2812.md)|WS2812b RGB LED|Yes|
|[Servo](docs/Servo.md)|Servo drive|Yes|
|[Stepper](docs/Stepper.md)|Stepper motor|Yes|
|[TSOP312](docs/TSOP312.md)|TSOP312xx IR receiver|Yes|
|[REncoder](docs/REncoder.md)|Rotary Encoder|No<sup>*</sup>|
|[HX711](docs/HX711.md)|Weighing sensor|No<sup>*</sup>|
|[W5500Lite](docs/W5500Lite.md)|W5500 Lite Ethernet module|No<sup>*</sup>|

<sup>*</sup>Due to the memory limit of the hardware you can not use all plugins simuatinely. 
For example, the [W5500Lite](docs/W5500Lite.md) plugin is deactivated by default. 
See [Plugin activation](#plugin-activation) section to activate/deactivate plugins.

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

#### Installation

* Create MultiSens folder in your Arduino sketch directory.
* Open it.
* Clone this repository.
* Run Arduino environment, compile the code and upload it to the board.

**Be careful!** This project was developed and tested for `Arduino Uno` board only!

```bash
mkdir MultiSens
cd MultiSens
git clone https://github.com/sersad-ru/MultiSens
```

#### Required dependencies

This software depens from Arduino standart libraries only. It uses `Software Serial`, `Wire` and
`SPI` libraries. All plugins code uses minimal subset of functions required only for testing sensors.
To take advantage of the full range of sensor capabilities, you'll need full-size libraries from
the Arduino community.

For example, all RFID plugins only reads manufacturer predefined card ID. 
No functions for key manipulation or data writing. 
All plugins for temperature sensors can read current temperature only and 
have no functions to setup sensor resolution and so on.

#### User interface

<p align="center"><img src="docs/Buttons.jpg"/></p>

The MultiSens has 16x2 LCD display and six buttons:
`SELECT`, `RESET`, `LEFT`, `RIGHT`, `UP` and `DOWN`.

Some button functions depends from plugin logic and some have system-wide functionality.

* The `LEFT` and `RIGTH` buttons are always used to scroll the screen. 
* To scoll the display to the start position, press and hold the `LEFT` button.
* The `RESET` button always terminates the current plugin and reboot the system.
* Holding the `DOWN` button and pressing the `RESET` button will erase **all** plugins 
settings in `EEPROM`.
* In the root menu (after reboot) you can see the system name and a list of registered plugins.
You can select a plugin using the `UP` and `DOWN` buttons. To launch the plugin, press the
`SELECT` button.
* If you hold the `SELECT` button, the current plugin number will be stored in `EEPROM`. 
The plugins list will be scrolled automatically to stored position after reboot. To clear
this setting you can erase all settings by holding the `DOWN` button and pressing the `RESET` button,
or you can just select plugin number one and store it to `EEPROM`.
* The `UP`, `DOWN` and `SELECT` buttons usually used inside plugins to change values and to start
actions.  


#### Plugin activation

Due to the memory limit of the hardware you may need to activate/deactivate some plugins.
To do this, open [MultiSens.ino](/MultiSens.ino) and comment required line in plugin registration
section. After that recompile the code and upload it to the board. 

```cpp
// Registered plugins
MultiSensPlugin plugins[] = {

  // Active plugin
  {&plgFindPin,       "FindPin",          0},  
  // Active plugin
  {&plgDigitalAnalog, "DigAn Read",       sizeof(plgDigitalAnalogCfg)}, 

  // Inactive plugin
  // {&plgDigitalRead,   "DigitalRead",      sizeof(plgDigitalReadCfg)},
  // Inactive plugin 
  // {&plgAnalogRead,    "AnalogRead",       sizeof(plgAnalogReadCfg)}, 

...

  {&plgRC522,         "RC522 (13.56MHz)", 0},  // Active plugin   
}; // 

```

**Attention!** Don't forget to clear `EEPROM` settings after changing registered plugin 
list or order of plugins in this list. To do this hold down the `DOWN` button then press the 
`RESET` button.


## Adding plugins
In there are no plugin for your sensor you can create your own plugin and add it to the MultiSens. 
Maybe you will need to [deactivate](#plugin-activation) some existent plugins to free device memory.

You can create your own plugin in 3 steps.

### Step 1. The plugin header
First of all choose the plugins name. Plugin files and main function name should start with "plg".  
For example, `plgSample.h` for plugin named "Sample".
Let's look into `plgSample.h`.
```cpp
#pragma once
#include <Arduino.h>
#include "mscore.h"  // Include the MultiSens Core

/*
* The Sample plugin reads the digital pin P0 and prints it current state. 
* Press SELECT button to start/stop reading process.
*/

/* The main plugin function declaration */
void plgSample();
```
In this file we include standard arduino header `#include <Arduino.h>` and MuiltSens Core header `#include "mscore.h"`.
Then we add some plugin description in comment and declare the main plugin function `void plgSample();`. 
This function should no return or accept any params and should never terminate. 
Switching from one plugin to another always occurs through a device reset.


### Step 2. The plugin code
The plugin code must be placed in the `plgSample.cpp` file.
```cpp
#include "plgSample.h"

#define INPUT_PIN P0
#define READ_DELAY_MS 500 // 0.5 seconds between attempt

// == Main plugin function ==
void plgSample(){

  // Init    
  pinMode(INPUT_PIN, INPUT);
  
  // Dispaly init
  core.moveCursor(0, 1); // First symbol of second line

  core.print(MS_SYM_SELECT_CODE); // The SELECT button symbol
  core.println(F("-to start/stop"));

  uint8_t can_read = false; // Read enable flag
  uint8_t value;

  // Main loop
  while(1){
    // Process user input    
    switch (core.getButton()) {
      case SELECT: // React on SELECT button
        can_read = !can_read; // enable/disable reading
      break; 
    
      default: break;
    }//switch

    if(!can_read) continue; // reading is disabled

    // Read results
    value = digitalRead(INPUT_PIN);

    // Display results on the screen
    core.moveCursor(0, 1); // First symbol of second line
    core.print(F("Value: "));
    core.println(value);

    // Send results to the Serial
    Serial.print(F("Value: "));
    Serial.println(value);

    delay(READ_DELAY_MS); // Delay between attemps
  }//while
}//plgSample
```

First of all, let's add the plugin header `#include "plgSample.h"`.
Then declare an input pin and a delay between plugin activities.
```cpp
#define INPUT_PIN P0
#define READ_DELAY_MS 500
```

Next part is the main plugin function.
This function should no return or accept any params and should never terminate. 
How to write your own plugin.
```cpp
void plgSample(){
```
In the init section of this function we should configure plugin pins. `pinMode(INPUT_PIN, INPUT);`
Then we need to send the welcome message to the display.
```cpp
core.moveCursor(0, 1); // First symbol of second line

core.print(MS_SYM_SELECT_CODE); // The SELECT button symbol
core.println(F("-to start/stop"));
```
The [`core.moveCursor`](docs/Core.md#move-cursor) function moves the screen cursor to the fisrt position of the second line.
The first line of the screen always contains current plugin name and is not available for plugins.

The `core.print` function prints the data at the current cursor position.
`MS_SYM_SELECT_CODE` is the predefined symbol to display `SELECT` button. Other predefined symbols 
are in [mscustomsymbols.h](mscustomsymbols.h).
More information about MuitiSens Core functions are in [MultiSens Core API](docs/Core.md).

Next important part is the main infinity cycle.
```cpp
// Main loop
while(1){
```
The main plugin function should never terminate. Switching from one plugin to another always occurs through a device reset. So we use an infinity loop.
In this loop, we firstly check for the user input.
```cpp
switch (core.getButton()) {
  case SELECT: // React on SELECT button
    can_read = !can_read; // enable/disable reading
  break; 
  
  default: break;
}//switch
```
The [`core.getButton`](docs/Core.md#get-the-button-code) function returns the [`button code`](docs/Core.md#button-codes) if the button was pressed.
We react on this event by setting or resetting `can_read` flag.
Next we finish this iteration of the cycle `if(!can_read) continue;` or read and display current pin state.
```cpp
// Read results
value = digitalRead(INPUT_PIN);

// Display results on the screen
core.moveCursor(0, 1); // First symbol of second line
core.print(F("Value: "));
core.println(value);
```
To send current value of the pin to the Serial we use `Serial.println(value);` from the standard arnuino library.

The last part of the main loop is the delay between attempts `delay(READ_DELAY_MS);`.


### Step 3. The plugin activation
To activate the plugin open [`MultiSens.ino`](/MultiSens.ino) file.
Then include the plugin header at the end of plugins header list.
```cpp
#include "plgSample.h"
```
Find the plugins registration section and append the registration record to the end of the list.
```cpp
// Registred plugins
MultiSensPlugin plugins[] = {
 ...
  {&plgSample,        "Sample Plugin",  0},   
};  

```
* The first param of the registration record is the link to the main plugin function `&plgSample`.
* The second one is the plugin title `"Sample Plugin"`. This title will be displayed in the main menu and on the first line of the screen when the plugin is launched.
* The last param is the size of plugins settings block. The Sample plugin use no settings block, so we sets is to **zero**.
More about settings block and how to store it to the EEPROM see in [`EEPROM Functions`](docs/Core.md#eeprom-functions) section of the [MultiSens Core API](docs/Core.md).

The last thing we need to do is to save [`MultiSens.ino`](/MultiSens.ino), recompile it and send it to the device. 

And of course, pray that we have no errors in the code and that everything works fine. :)