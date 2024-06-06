# MultiSens Core API

All functions are available through the static `core` object.

```cpp
core.print("Hello World");
```

## Table of contents
* [MultiSens Pins](#multisens-pins)
* [Screen Functions](#screen-functions)
* [String Functions](#string-functions)
* [EEPROM Functions](#eeprom-functions)
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
with [`restoreCursor`](#restore-cursor) function.


### Restore cursor
```cpp
void restoreCursor();
```
Restore previously stored cursor position. See  [`storeCursor`](#store-cursor) function.

```cpp
moveCursor(0, 1); // Place the cursor to the first symbol of the second line.
storeCursor();    // Store current cursor position
print("foo");     // Print some text. Current cursor will be at [3, 1]
restoreCursor();  // Now cursor is back to [0, 1]
print("bar");     // This text will replace "foo"

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
For example, if you need to print **-25.7**, `value` should be **-257** and `scale` should be **10**.

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



## EEPROM functions

### Save settings
```cpp
void saveSettings(uint8_t * data);
```

Saves plugin settings block in `EEPROM`.  This block can be restored using [`loadSettings`](#load-settings) function.
The size of the settings block **MUST** be provided in plugin registration process.

**ATTENTION!** If you register new a plugin or change the size of the settings block of any 
registered plugin you **MUST** clear all settings stored in `EEPROM`. Otherwise you will get
a wrong values. To do this hold the `DOWN` button and press `RESET` to clear stored settings of all plugins. 

|Prarm|Type|Description|
|:---:|:---|:---|
|data|pointer to `uint_8t`|Pointer to the settings block|
                                                        
```cpp
// Define the plugin settings block in the plugin header file
struct{
  uint8_t value; // Stored value
} plgMyPluginSettings;


// Register the plugin with size of settings block in MultiSens.ino
MultiSensPlugin plugins[] = {
  {&plgMyPlugin,  "MyPlugin",  sizeof(plgMyPluginSettings)},
  ...
};

// Update stored value
plgMyPluginSettings.value = 42;

// Save the plugin settings block
core.saveSettings((uint8_t*)&plgMyPluginSettings);
```


### Load settings
```cpp
bool loadSettings(uint8_t * data);
```
Loads the plugin settings block from the `EEPROM` previously saved with [`saveSettings`](#save-settings) function.
The size of the settings block **MUST** be provided in plugin registration process.

|Prarm|Type|Description|
|:---:|:---|:---|
|data|pointer to `uint_8t`|Pointer to the settings block|

Function returns **true** if settings was successfully loaded. Otherwise it returns **false**.

**ATTENTION!** If you register a new plugin or change the size of the settings block of any 
registered plugin you **MUST** clear all settings stored in `EEPROM`. Otherwise you will get
a wrong values. To do this hold the `DOWN` button and press `RESET` to clear stored settings of all plugins. 

```cpp
// Define the plugin settings block in the plugin header file
struct{
  uint8_t value; // Stored value
} plgMyPluginSettings;


// Register the plugin with size of settings block in MultiSens.ino
MultiSensPlugin plugins[] = {
  {&plgMyPlugin,  "MyPlugin",  sizeof(plgMyPluginSettings)},
  ...
};

// Update stored value
plgMyPluginSettings.value = 42;

// Save the plugin settings block
core.saveSettings((uint8_t*)&plgMyPluginSettings);

// Try to load previously stored settings
if(!core.loadSettings((uint8_t*)&plgMyPluginSettings)){
    // Settings was not loaded. Use default values.
}//if  
// Settings was loaded successfully

```



## I<sup>2</sup>C Utils

### Write register
```cpp
void i2cWriteReg(const uint8_t i2c_addr, const uint8_t reg, const uint32_t val, const MultiSensI2CRegSize reg_size = SIZE_8);
```
Writes data to the register of device using I<sup>2</sup>C bus.
**ATTENTION!** `Wire.begin()` method should be called by plugin before this function.

|Prarm|Type|Description|
|:---:|:---|:---|
|i2c_addr|`uint8_t`|Device address on I<sub>2</sup>C bus|
|reg|`uint8_t`|Register address|
|val|`uint32_t`|Value|
|reg_size|`MultiSensI2CRegSize`|Register size. From **SIZE_8** to **SIZE_32**|


```cpp
typedef enum {
  SIZE_0  = 0,
  SIZE_8  = 1,
  SIZE_16 = 2,
  SIZE_24 = 3,
  SIZE_32 = 4
} MultiSensI2CRegSize;

```

```cpp
Wire.begin();
...
// Writes 0x08 to the register number 0x2D on device with address 0x53 
core.i2cWriteReg(0x53, 0x2D, 0x08);

// Writes 0x08AB to the register number 0x2D on device with address 0x53 
core.i2cWriteReg(0x53, 0x2D, 0x08AB, SIZE_16);

```


### Read register
```cpp
uint32_t i2cReadReg(const uint8_t i2c_addr, const uint8_t reg, const MultiSensI2CRegSize reg_size = SIZE_8);
```
Reads data from the register of device using I<sup>2</sup>C bus.
**ATTENTION!** `Wire.begin()` method should be called by plugin before this function.

|Prarm|Type|Description|
|:---:|:---|:---|
|i2c_addr|`uint8_t`|Device address on I<sub>2</sup>C bus|
|reg|`uint8_t`|Register address|
|reg_size|`MultiSensI2CRegSize`|Register size. From **SIZE_8** to **SIZE_32**|

```cpp
typedef enum {
  SIZE_0  = 0,
  SIZE_8  = 1,
  SIZE_16 = 2,
  SIZE_24 = 3,
  SIZE_32 = 4
} MultiSensI2CRegSize;

```

Function returns the value of the register.

```cpp
Wire.begin();
...
// Reads 24 bit value from register number 0xFA on device with address 0x77
int32_t val = core.i2cReadReg(0x77, 0xFA, SIZE_24);
```


### Request bytes from the register
```cpp
void i2cRequestRead(const uint8_t i2c_addr, const uint8_t reg, const uint8_t cnt);
```

Sends register address and request specified number of bytes to be readed. `Wire.read()` method should to be called next to this function.

|Prarm|Type|Description|
|:---:|:---|:---|
|i2c_addr|`uint8_t`|Device address on I<sub>2</sup>C bus|
|reg|`uint8_t`|Register address|
|cnt|`uint8_t`|Amount of bytes to be read|

```cpp
uint16_t x, y, z;
Wire.begin();
// Request 6 bytes from the register number 0x32 from the device with address 0x53
core.i2cRequestRead(0x53, 0x32, 0x06); 
x = Wire.read() | Wire.read() << 8; // Read first 2 bytes of 6
y = Wire.read() | Wire.read() << 8; // Read next 2 bytes of 6
z = Wire.read() | Wire.read() << 8; // Read last 2 bytes of 6

```
