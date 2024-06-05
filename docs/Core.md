# MultiSens Core API

All functions are available through the static `core` object.

```cpp
core.print("Hello World");
```

## Table of contents
* [MultiSens Pins](#multisens-pins)
* [Screen Functions](#screen-functions)
* [String Functions](#string-functions)
* [I<sup>2</sup>C Utils](#i2c-utils)

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
```cpp
void clear();
```
This function clears the display. All symbols will be erased including hidden columns.


### Move home
```cpp
void home();
```
This function moves the cursor and scroll the display to the start position [0,0].


### Move cursor
```cpp
void moveCursor(uint8_t col, uint8_t row);
```
This function moves the cursor to the selected position.
|Prarm|Type|Description|
|:---:|:---|:---|
|col|`uint8_t`|Colum number from **0** to **39**|
|row|`uint8_t`|Row number from **0** to **1**|

```cpp
 moveCursor(0, 1); // Place the cursor to the first symbol of the second line.
```


### Store cursor
```cpp
void storeCursor();
```
Saves the current cursor position in the internal buffer. Cursor can be restored later 
with [`restoreCursor`](restore-cursor) function.


### Restore cursor
```cpp
void restoreCursor();
```
Restore previously stored cursor position. See  [`storeCursor`](store-cursor) function.

```cpp
moveCursor(0, 1); // Place the cursor to the first symbol of the second line.
storeCursor(); // Store current cursor position
print("foo"); // Print some text. Current cursor will be at [3, 1]
restoreCursor(); // Now cursor is back to [0, 1]
print("bar"); // This text will replace "foo"

```

### Set cursor type
```cpp
void setCursorType(MultiSensCursor ct);
```
This function sets display cursor to the specified type.
Available cursor types are:

```cpp
typedef enum {
  OFF,        // No cursor
  UNDERLINE,  // Underline cursor
  BLOCK       // Block cursor
} MultiSensCursor;

```

```cpp
setCursorType(UNDERLINE); // Set cursor to underline type
```



## String functions

### Get the name of pin
```cpp
char * getPinName(uint8_t pinNumber);
```

Returns the pointer to the string in the internal buffer with the name of the selected [pin](#muiltsens-pins). 
```cpp
print(getPinName(P1)); // Prints: "P1"
print(getPinName(P3)); // Prints: "P3"
```

### Get the string aligned to the right
```cpp
char * rAlign(uint32_t val, const uint8_t width, const char fill = ' ', uint8_t base = DEC);
char * rAlign(int32_t val, const uint8_t width, const char fill = ' ', uint8_t base = DEC);
char * rAlign(uint16_t val, const uint8_t width, const char fill = ' ', uint8_t base = DEC);
char * rAlign(int16_t val, const uint8_t width, const char fill = ' ', uint8_t base = DEC);
char * rAlign(uint8_t val, const uint8_t width, const char fill = ' ', uint8_t base = DEC);
char * rAlign(int8_t val, const uint8_t width, const char fill = ' ', uint8_t base = DEC);
```

Returns the pointer to the string representation of the `val` paramert in the internal buffer. 

|Prarm|Type|Description|
|:---:|:---|:---|
|val|From `uint8_t` to `uint32_t`|Any integer value|
|width|`uint8_t`|String width from **1** to **32**|
|fill|`char`|Leading fill symbol|
|base|`uint8_t`|Number base **DEC**, **HEX**, **OCT** or **BIN**|

```cpp
print(rAlign(42, 5));           // Prints: "   42"
print(rAlign(-42, 5));          // Prints: "  -42"
print(rAlign(4242, 5));         // Prints: " 4242"
print(rAlign(42, 5, '@'));      // Prints: "@@@42"
print(rAlign(42, 5, ' ', HEX)); // Prints: "   2A"
```


### Print values with fixed decimal point
```cpp
void printValScale(Print &p, int32_t value, int16_t scale = 10);
```

Prints `value` with fixed decimal point to the selected stream.
For example, if you need to print -25.7, `value` should be **-257** and `scale` should be **10**.

|Prarm|Type|Description|
|:---:|:---|:---|
|p|`Print`|Stream to print to|
|value|`int32_t`|Scaled value|
|scale|`int16_t`|Scale factor|

```cpp
printValScale(core, -257);      // Prints: "-25.7" to the device screen
printValScale(core, 254);       // Prints: "25.4" to the device screen
printValScale(Serial, 254);     // Prints: "25.4" to the serial
printValScale(core, 2545);      // Prints: "254.5" to the device screen
printValScale(core, 2545, 100); // Prints: "25.45" to the device screen
printValScale(core, 254, 100);  // Prints: "2.54" to the device screen
```


### Print array of bytes in HEX
```cpp
void printHexArray(Print &p, uint8_t* arr, const uint8_t arrSize, const char spacer = ':');   
```

Prints the array of hexadecimal values to the selected stream with the specified separator.

|Prarm|Type|Description|
|:---:|:---|:---|
|p|`Print`|Stream to print to|
|arr|pointer to `uint8_t`|Array of bytes|
|arrSize|`uint8_t`|Array size|
|spacer|`char`|Item separator|

```cpp
uint8_t arr[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x00};
printHexArray(core, arr, 6);      // Prints: "DE:AD:BE:EF:00:00"
printHexArray(core, arr, 6, '-'); // Prints: "DE-AD-BE-EF-00-00"
```


### Print long interger as array
```cpp
void printLongAsArray(Print &p, const uint32_t val, const char spacer = ':');
```

Prints `val` as array of four bytes to the selected stream using specified separator.

```cpp
printLongAsArray(core, 0xAABBCCDD);      // Prints: "AA:BB:CC:DD"
printLongAsArray(core, 0xAABBCCDD, '-'); // Prints: "AA-BB-CC-DD"
```


## I<sup>2</sup>C Utils

