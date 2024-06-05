# MultiSens Core API

All functions are available through the static `core` object.

```cpp
core.print("Hello World");
```

## Table of contents
* [MultiSens Pins](#multisens-pins)
* [Screen Functions](#screen-functions)
* [String Functions](#string-functions)

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
|col|uint8_t|Colum number from **0** to **39**|
|row|uint8_t|Row number from **0** to **1**|

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
setCursorType(UNDERLINE);
```



## String functions

### Get the name of pin
```cpp
char * getPinName(uint8_t pinNumber);
```

Returns the pointer to the string in the internal buffer with the name of selected pin. 
```cpp
print(getPinName(P1)); // Prints: "P1"
```