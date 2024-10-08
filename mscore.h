/*
Arduino Multi Senstor Tester
(c)2023-2024 by Sergey Sadovnikov (sersad@gmail.com)

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

#pragma once
#include <inttypes.h>
#include <stdio.h>
#include <Print.h>

#include "mspinmap.h"
#include "mscustomsymbols.h"
#include "flashcfg.h"

// MultiSens Pins

#define P0 P(0) // 2 Digital - INT0       - Green
#define P1 P(1) // 3 Digital - INT1 - PWM - Blue
#define P2 P(2) // 11 Digital - MOSI - PWM - Yellow
#define P3 P(3) // 12 Digital - MISO       - Violet
#define P4 P(4) // 13 Digital - SCK        - Brown
#define P5 P(5) // 17 Analog               - Orange
#define P6 P(6) // 18 Analog  - SDA        - Yellow-Black
#define P7 P(7) // 19 Analog  - SCL        - Gray-Black

const uint16_t MS_STD_DELAYS[] PROGMEM = {10, 50, 100, 250, 500, 1000, 1500, 2000}; // Variants of delays

const char MS_MSG_READ_ERROR[] PROGMEM = "** Read error **";
const char MS_MSG_DELAY_END[]  PROGMEM = "ms)";
const char MS_MSG_DELAY_NAME[] PROGMEM = "D: ";

class MultiSensCore;

// Plugin function type (MultiSensCore object is available globally as 'core')
typedef void (*PluginFunction)(void);


// Plugin element
typedef struct {
  PluginFunction run; //Plugin function
  const char* title;  //Plugin title
  uint8_t cfg_size;   //Size (in bytes) of plugin settings block in EEPROM
} MultiSensPlugin;


// Button codes
#define MS_BTN_LONG_MASK 0x08 // Long pressed button flag
typedef enum {
  NONE,
  SELECT,
  UP,
  DOWN,  
  LEFT,  // Used internally for screen scrolling 
  RIGHT, // Used internally for screen scrolling 
  SELECT_LONG = SELECT | MS_BTN_LONG_MASK,
  UP_LONG     = UP     | MS_BTN_LONG_MASK,
  DOWN_LONG   = DOWN   | MS_BTN_LONG_MASK,
  LEFT_LONG   = LEFT   | MS_BTN_LONG_MASK, // Used internally for screen scrolling 
  RIGHT_LONG  = RIGHT  | MS_BTN_LONG_MASK  // Used internally for screen scrolling 
} MultiSensButton;


// Cursor types
typedef enum {
  OFF,
  UNDERLINE,
  BLOCK
} MultiSensCursor;


//I2C register size
typedef enum {
  SIZE_0  = 0,
  SIZE_8  = 1,
  SIZE_16 = 2,
  SIZE_24 = 3,
  SIZE_32 = 4
} MultiSensI2CRegSize;


#define arraySize(_array) ( sizeof(_array) / sizeof(*(_array)) )
// Макрос для приведения PROGMEM строк к const __FlashStringHelper*
#define FF(val) ((const __FlashStringHelper*)val)
  
class MultiSensCore : public Print {
public:
  //** Constructor
  MultiSensCore();

  //** Initialization
  //* baud - Baudrate for serial port logs
  void init(MultiSensPlugin* plugins, uint8_t pluginsCount, uint32_t baud = 9600);


  //** Runs system menu
  void menu();
  
  //** Set cursor type and turn on the screen
  //* ct - new cursor type (OFF, UNDERLINE, BLOCK)
  void setCursorType(MultiSensCursor ct);

  //** Clear the display
  void clear();

  //** Move cursor to 0,0 and unshift display
  void home();

  //** Move cursor to col, row position [0..39, 0..1]
  void moveCursor(uint8_t col, uint8_t row);

  //** Save current cursor position
  void storeCursor();

  //** Restore previously stored cursor position
  void restoreCursor();
  
  //** Create new symbol from array. Symbols with code from 0 to 2 are available
  //* code - custom symbol code
  //* data - custom symbol data array (see mscustomsymbols.h) 
  void createSymbol(const uint8_t code, PMultiSensSymbol data);
  
  //** Returns single button code if the button was short pressed, and repeats button codes for long pressed button
  MultiSensButton getButton();

  //** Waits for button and returns its code. This function blocks until the button is pressed for long or pressed and released for short.
  MultiSensButton wait4Button();

  //** Returns the code of pressed button. Repeats this code until the button is pressed.
  MultiSensButton buttonPressed();

  //** Returns the code of released button. Returns NONE until the button is pressed but not released.
  MultiSensButton buttonReleased();


  //** Saves settings block in EEPROM. Size of the block MUST be provided in MultiSensPlugin structire.
  //* data - pointer for settings block
  void saveSettings(uint8_t * data);

  //** Loads settings block from EEPROM. Size of the block MUST be provided in MultiSensPlugin structire.
  //* Returns: False if settings was reseted and default values should be used. True if not. 
  //* data - pointer for settings block
  bool loadSettings(uint8_t * data);


  //** Returns pointer to string n format "Px: " where x - pin number from 0 to 7
  //* pinNumber - pin number from 0 to 7
  char * getPinName(uint8_t pinNumber);


  //** Returns pointer to string representation of val, aligned right.
  //* val - number
  //* width - string width
  //* fill - leading fill symbol
  //* base - number base (DEC, HEX, OCT, BIN) 
  char * rAlign(uint32_t val, const uint8_t width, const char fill = ' ', uint8_t base = DEC);
  char * rAlign(int32_t val, const uint8_t width, const char fill = ' ', uint8_t base = DEC);
  char * rAlign(uint16_t val, const uint8_t width, const char fill = ' ', uint8_t base = DEC);
  char * rAlign(int16_t val, const uint8_t width, const char fill = ' ', uint8_t base = DEC);
  char * rAlign(uint8_t val, const uint8_t width, const char fill = ' ', uint8_t base = DEC);
  char * rAlign(int8_t val, const uint8_t width, const char fill = ' ', uint8_t base = DEC);


  //** Prints integer value as a float. Value must be multiplyed by base. If you need to print -25.7, value shold be = -257 and scale = 10
  void printValScale(Print &p, int32_t value, int16_t scale = 10);


  //** Prints array of bytes in HEX using specified spacer
  //* arr - array of bytes
  //* arrSize - size of array
  //* spacer - spacer symbol.
  void printHexArray(Print &p, uint8_t* arr, const uint8_t arrSize, const char spacer = ':');   


  //** Prints uint32_t as array of four bytes in HEX using secified spacer 
  //* val - 32-bit unsigned value
  //* spacer - spacer symbol.
  void printLongAsArray(Print &p, const uint32_t val, const char spacer = ':');
  
  virtual size_t write(uint8_t value);
  using Print::write;

  //I2C utils. ATTENTION!  Wire.begin() should be called from plugin
  //** Write I2C register
  //* i2c_addr - address of I2C device
  //* reg - register number
  //* val - register value
  //* reg_size - register size (SIZE_8, SIZE_16, SIZE_24 or SIZE_32 bits). Big endian (сначала старший)  
  void i2cWriteReg(const uint8_t i2c_addr, const uint8_t reg, const uint32_t val, const MultiSensI2CRegSize reg_size = SIZE_8);// Big endian (сначала старший)

  //** Read I2C register
  //* i2c_addr - address of I2C device
  //* reg - register number
  //* reg_size - register size (SIZE_8, SIZE_16, SIZE_24 or SIZE_32 bits). Big endian (сначала старший)
  uint32_t i2cReadReg(const uint8_t i2c_addr, const uint8_t reg, const MultiSensI2CRegSize reg_size = SIZE_8);// Big endian (сначала старший)

  //** Send I2C register address and waits for reading. Wire.read() should be next call 
  //* i2c_addr - address of I2C device
  //* reg - register number
  //* cnt - number of requested bytes
  void i2cRequestRead(const uint8_t i2c_addr, const uint8_t reg, const uint8_t cnt);
  
  void _btn_isr(); // Обработчик перывания, читающий кнопки
private:
  MultiSensPlugin * _plugins; // Массив плагинов 
  uint8_t _pluginsCount; // Количество элементов массива плагинов
  int8_t _mnu_current; // Текущий пункт меню (запущенный плагин)

  flashcfg _cfg; // Сохраняемые во EEPROM настройки
  char _printBuf[8 * sizeof(int32_t) + 1]; // Буфер для подготовки строк

  volatile MultiSensButton _btn_pressed_code; // Текущий код нажатой кнопки
  volatile MultiSensButton _btn_released_code; // Текущий код отпущенной кнопки
  volatile uint8_t _btn_pressed_count;  // Сколько вызовов таймера кнопка остается нажатой
  volatile uint8_t _btn_released_count; // Сколько вызовов таймера кнопка была нажатой до отпускания
  
  uint8_t _lcd_cursor_offset; // Текущее положение курсора в буфере
  uint8_t _lcd_stored_cursor; // Тут можно сохранить позицию курсора и потом давать кучу принтов в одно место, используя lcdRestoreCursor;  
  
  MultiSensCursor _lcd_cursor_type; // Текущий тип курсора
  MultiSensCursor _lcd_stored_cursor_type; // Сохраненный тип курсора

  char * _rAlign(uint32_t val, uint8_t width, const char fill, uint8_t base, uint8_t isNegative); // Преобразует число в строку с выравниванием вправо.
  void _run_plugin(); // Запустить плагин
  uint16_t _cfg_calc_offset(); // Счиает смещение блока настроке для текущего плагина
  int _btn_read(); // Читает аналоговый пин, к которому подключены кнопки
  MultiSensButton _btn_analog2btn(int code); // Преобразовать результат аналогового чтения к коду кнопки
    
  void _lcd_create_symbol(const uint8_t code, PMultiSensSymbol data); // Загрузить свой символ
  void _lcd_scroll_left(); // Сдвинуть экран влево (кольцевой сдвиг)
  void _lcd_scroll_right(); // Сдвинуть экран вправо (кольцевой сдвиг)

  void _lcd_command(uint8_t value); // Отправить команду дисплею
  void _lcd_data(uint8_t value); // Отправить данные дисплею
  void _lcd_send_byte(uint8_t value); // Отправить байт на шину данных дисплея
  void _lcd_send_half_byte(uint8_t value); //Отправить полбайта (младшие) на шину данных дисплея
}; //class

extern MultiSensCore core;
