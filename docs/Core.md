# MultiSens Core API

## Table of contents
* [MultiSens Pins](#multisens-pins)
* [Screen Functions](#screen-functions)

## MuiltSens Pins
You can up to 8 pins in your plugins according to the following table.

|MultiSens Pin|Color|Function|
|:---:|:---|:---|
|P0|Green|Digital I/O, INT0|
|P1|Blue|Digital I/O, INT1, PWM|
|P2|Yellow|Digital I/O, MOSI, PWM|
|P3|Violet|Digital I/O, MISO|
|P4|Brown|Digital I/O, SCK|
|P5|Orange|Analog In|
|P6|Yellow-Black|Analog In, SDA|
|P7|Gray-Black|Analog In, SCL|


## Screen functions
The MultiSens screen has **2** lines with **16** visible columns. In addition, there are 24 hidden 
columns. You can scroll to them using the `LEFT` and `RIGHT` buttons.


### Clear the display
```
void clear();
```
This function clears the display. All symbols will be erased including hidden columns.


### Move home
```
void home();
```
This function moves the cursor and scroll the display to the start position [0,0].