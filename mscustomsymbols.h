#pragma once

/*
* Дополнительные символы знакогенератора
* http://microsin.net/programming/avr/Custom%20Character%20Generator%20for%20HD44780%20LCD%20Modules.html?ysclid=lohp15fplm806725045
*/

// Custom Symbols
typedef const PROGMEM uint8_t MultiSensSymbol;
typedef MultiSensSymbol* PMultiSensSymbol;


// Коды символов из ПЗУ
#define MS_SYM_ARROW_LEFT_CODE  (char)0x7e  // Стрелка влево
#define MS_SYM_ARROW_RIGHT_CODE (char)0x7f  // Стрелка вправо
#define MS_SYM_DEGREE_CODE      (char)0xdf  // Градус
#define MS_SYM_OMEGA_CODE       (char)0xf4  // Омега
#define MS_SYM_MICRO_CODE       (char)0xe4  // Микро
#define MS_SYM_INFINITY_CODE    (char)0xf3  // Бесконечность
#define MS_SYM_PI_CODE          (char)0xf7  // Пи
#define MS_SYM_SUM_CODE         (char)0xf6  // Сумма
#define MS_SYM_AVERAGE_CODE     (char)0xf8  // Среднее
#define MS_SYM_DIVIDE_CODE      (char)0xfd  // Знак деления
#define MS_SYM_PROGRESS_CODE    (char)0xff  // Полностью заполненное знакоместо для прогрессбара


// Коды дополнительных символов
#define MS_SYM_SQUARED_CODE      (char)0x01 // В квадрате
#define MS_SYM_BACKSLASH_CODE    (char)0x02 // Обратный слеш 
#define MS_SYM_RESET_CODE        (char)0x03 // Сброс
#define MS_SYM_SELECT_CODE       (char)0x04 // Выбор
#define MS_SYM_ARROW_UPDOWN_CODE (char)0x05 // Стрелка вверх и вниз
#define MS_SYM_ARROW_DOWN_CODE   (char)0x06 // Стрелка вниз
#define MS_SYM_ARROW_UP_CODE     (char)0x07 // Стрелка вверх


// Дополнительные символы
// В квадрате (в смысле вторая степень)
MultiSensSymbol MS_SYM_SQUARED[] = {
  0b01100,
  0b10010,
  0b00100,
  0b01000,
  0b11110,
  0b00000,
  0b00000,
  0b00000
};

// Обратный слеш
MultiSensSymbol MS_SYM_BACKSLASH[] = {
  0b00000,
  0b10000,
  0b01000,
  0b00100,
  0b00010,
  0b00001,
  0b00000,
  0b00000
};

// Сброс
MultiSensSymbol MS_SYM_RESET[] = {
  0b00000,
  0b01110,
  0b10001,
  0b10001,
  0b10001,
  0b01110,
  0b00000,
  0b00000
};

// Выбор
MultiSensSymbol MS_SYM_SELECT[] = {
  0b00000,
  0b11111,
  0b10001,
  0b10001,
  0b10001,
  0b11111,
  0b00000,
  0b00000
};

// Стрелка вверх и вниз
MultiSensSymbol MS_SYM_ARROW_UPDOWN[] = {
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b10101,
  0b01110,
  0b00100,
  0b00000
}; 

// Стрелка вниз
MultiSensSymbol MS_SYM_ARROW_DOWN[] = {
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b10101,
  0b01110,
  0b00100,
  0b00000
};

// Стрелка вверх
MultiSensSymbol MS_SYM_ARROW_UP[] = {
  0b00100,
  0b01110,
  0b10101,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b00000
};
