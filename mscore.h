#pragma once
#include <inttypes.h>
#include <stdio.h>
#include <Print.h>

#include "mscustomsymbols.h"
#include "flashcfg.h"

// MultiSens Pins

class MultiSensCore;

// Plugin functione type
typedef void (*PluginFunction)(MultiSensCore& core);


// Plugin element
typedef struct {
  PluginFunction run; //Plugin function
  const char* Title; //Plugin title
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

#define arraySize(_array) ( sizeof(_array) / sizeof(*(_array)) )
  
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

  //** Clear display
  void clear();

  //** Move cursor to 0,0 and unshift display
  void home();

  //** Move cursor to col, row position [0..39, 0..1]
  void moveCursor(uint8_t col, uint8_t row);

  //** Save current cursor position
  void storeCursor();

  //** Restore previously srored cursor position
  void restoreCursor();
  
  //** Create new symbol from array. Symbols with code from 0 to 2 are available
  //* code - custom symbol code
  //* data - custom symbol data array (see mscustomsymbols.h) 
  void createSymbol(const uint8_t code, PMultiSensSymbol data);

  //** Waits for button and returns its code. This function blocks until the button is released.
  MultiSensButton getButton();

  //** Returns the code of pressed button. Repeats this code until the button is pressed.
  MultiSensButton buttonPressed();

  //** Returns the code of released button. Returns NONE until the button is pressed but not released.
  MultiSensButton buttonReleased();
  
  virtual size_t write(uint8_t value);
  using Print::write;
  
  void _btn_isr(); // Обработчик перывания, читающий кнопки
private:
  MultiSensPlugin * _plugins; // Массив плагинов 
  uint8_t _pluginsCount; // Количество элементов массива плагинов
  flashcfg _cfg; // Сохраняемые во EEPROM настройки

  volatile MultiSensButton _btn_pressed_code; // Текущий код нажатой кнопки
  volatile MultiSensButton _btn_released_code; // Текущий код отпущенной кнопки
  volatile uint8_t _btn_pressed_count;  // Сколько вызовов таймера кнопка остается нажатой
  volatile uint8_t _btn_released_count; // Сколько вызовов таймера кнопка была нажатой до отпускания
  
  uint8_t _lcd_cursor_offset; // Текущее положение курсора в буфере
  uint8_t _lcd_stored_cursor; // Тут можно сохранить позицию курсора и потом давать кучу принтов в одно место, используя lcdRestoreCursor;

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
