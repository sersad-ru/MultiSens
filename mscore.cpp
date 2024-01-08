#include "mscore.h"
#include "msdisp.h"
#include "msportman.h"
#include "buildid.h"

#include <Arduino.h>
#include <EEPROM.h>


// Начальный адрес хранения настроек в EEPROM
#define EEPROM_CFG_START 0

// Значения по умолчанию для хранимых в EEPROM настроек
#define DEFAULT_MENU_CURRENT 0

// Порт и пины Дисплея
#define LCD_DATA_PORT PORTD // Полупорт обмена данными D0-D4 (Старший полупорт D: PD4-PD7, пины 4-7)
#define LCD_DATA_PORT_CFG DDRD // Конфигурация порта данных
#define LCD_DATA_MASK 0xF0 // У порта используются старшие биты
#define LCD_CTRL_PORT PORTB // Порт пинов управления RS, EN, BL (PB0-PB2, пины 8-10)
#define LCD_CTRL_PORT_CFG DDRB // Конфигурация порта управления
#define LCD_RS_PIN 0 // Пин RS (PB0, пин 8)
#define LCD_EN_PIN 1 // Пин EN (PB1, пин 9)
#define LCD_BL_PIN 2 // Пин BL (PB2, пин 10)
#define LCD_PWM_REG TCCR1A // Порт таймера PWM для подстветки дисплея (PB2, TIMER1, канал B, штатный делитель /64)
#define LCD_PWM_BIT COM1B1 // Бит таймера PWM для подсветки дисплея (PB2, TIMER1, COM1B1)
#define LCD_PWM_COUNTER_REG OCR1B // Счетчик таймера PWM для подсветки диплея (канал B)
#define LCD_PWM_VALUE 0xff // Яркость экрана (полная)


// Пин кнопки
#define BTN_PIN 0 // Пин на аналоговом порту, к которому подключены кнопки (A0)

// Прерывание кнопки
#define BTN_MASK_REG TIMSK1 // Регист маски прерывания таймера для опроса кнопки (TIMER1, канал A, штатный делитеть /64)
#define BTN_MASK_BIT TOIE1  // Бит маски перерывания таймера для опроса кнопки (по переполнению 490 раз в секунду)

// Параметры обработки кнопки (обработчик вызывается по таймру 10 раз в секунду)
#define BTN_COUNT_MIN 1        // Минимальное количество вывовов таймера, чтобы засчитать нажатие (0,1 секунды)
#define BTN_COUNT_LONG 10      // Количество вызовов таймера, чтобы засчитать длинное нажатие (1 секунда)
#define BTN_COUNT_MAX 250      // Максимальное количество вызовов (что б счетчик через 0 не переходил)
#define BTN_LONG_REPEAT_MS 250 // Задержка (в ms) перед повторами удерживаемой кнопки (LONG) для getButton

const char MSG_MAIN_TITLE[] PROGMEM = "-=MultiSens=-";
const char MSG_CLEAR_SETTINGS[] PROGMEM = "Settings cleared.";
const char MSG_SAVE_SETTINGS[] PROGMEM = "Settings saved.";
const char MSG_PIN_NAME[] PROGMEM = "P0: ";

MultiSensCore::MultiSensCore(){
  _pluginsCount = 0;
  _btn_pressed_code = NONE; 
  _btn_released_code = NONE; 
  _btn_pressed_count = 0;
  _btn_released_count = 0;
}//constructor


//init
void MultiSensCore::init(MultiSensPlugin* plugins, uint8_t pluginsCount, uint32_t baud){
  _plugins = plugins;
  _pluginsCount = pluginsCount;
  _lcd_stored_cursor = 0;  

  // Грузим настройки из EEPROM
  EEPROM.get(EEPROM_CFG_START, _cfg);
  if(_cfg.noValue){ // В EEPROM ничего не было, устанавливаем значения по умолчанию
    _cfg.noValue = 0; //Выставляем флаг, что данные установлены
    _cfg.mnu_current = DEFAULT_MENU_CURRENT;
    EEPROM.put(EEPROM_CFG_START, _cfg);    
  }//if

  _mnu_current = _cfg.mnu_current; // Текущий пункт меню (из настроек, либо по умлочанию)
  
  // Инициализируем дисплей
  // Сначала порты
  _INT_OFF;
  _PORT_OUTPUT(LCD_CTRL_PORT_CFG, _BV(LCD_RS_PIN) | _BV(LCD_EN_PIN) | _BV(LCD_BL_PIN));
  _PORT_OUTPUT(LCD_DATA_PORT_CFG, LCD_DATA_MASK);
   // PWM для яркости
  _PORT_SET_HIGH(LCD_PWM_REG, _BV(LCD_PWM_BIT));  
  LCD_PWM_COUNTER_REG = LCD_PWM_VALUE;
  // Прерывания таймера для опроса клавиатуры
  _PORT_SET_HIGH(BTN_MASK_REG, _BV(BTN_MASK_BIT));  // По переполнению. По сравнению одновременно с PWM не работает. В результате 490 раз в секунду. 
  _INT_ON;

  // Ждем 15мс
  delayMicroseconds(15000); // Больше 16383 - функция косячит (https://www.arduino.cc/reference/en/language/functions/time/delaymicroseconds/)

  // Инитим и настраиваем экран
  _PORT_SET_LOW(LCD_CTRL_PORT, _BV(LCD_RS_PIN)); // RS вниз 
  _lcd_send_half_byte(LCD_INIT_4BIT); // Первая отправка команды инициализация 4-х битного режима
  delayMicroseconds(4500); // Ждем 4.1 ms
  _lcd_send_half_byte(LCD_INIT_4BIT); // Вторая отправка команды инициализация 4-х битного режима
  delayMicroseconds(100); //Ждем 100 ms
  _lcd_send_half_byte(LCD_INIT_4BIT); // Третья отправка команды инициализация 4-х битного режима
  delayMicroseconds(150); // Ждем 150
  _lcd_send_half_byte((LCD_F | LCD_F_4BIT | LCD_F_2LINE | LCD_F_5x8) >> 4); // Старшие 4 бита полной команды инициализации
  _lcd_command(LCD_F | LCD_F_4BIT | LCD_F_2LINE | LCD_F_5x8); // Полные настройки: шина, количество строк, шрифт
   setCursorType(OFF); // Включить экран и установить тип курсора
  _lcd_command(LCD_CMDS | LCD_CMDS_INC | LCD_CMDS_CUR);//Включить перемещение курсора слева направо
 
  // Загружаем свои символы
  _lcd_create_symbol(MS_SYM_SQUARED_CODE, MS_SYM_SQUARED);// Символ второй степени (квадрата)
  _lcd_create_symbol(MS_SYM_BACKSLASH_CODE, MS_SYM_BACKSLASH);// Символ обратной косой черты
  _lcd_create_symbol(MS_SYM_RESET_CODE, MS_SYM_RESET);// Символ сброса
  _lcd_create_symbol(MS_SYM_SELECT_CODE, MS_SYM_SELECT);// Символ выбора
  _lcd_create_symbol(MS_SYM_ARROW_UPDOWN_CODE, MS_SYM_ARROW_UPDOWN);// Символ стрелки вверх-вниз
  _lcd_create_symbol(MS_SYM_ARROW_DOWN_CODE, MS_SYM_ARROW_DOWN);// Символ стрелки вниз
  _lcd_create_symbol(MS_SYM_ARROW_UP_CODE, MS_SYM_ARROW_UP);// Символ стрелки вверх
  
   
  // Выводим стартовое сообщение
  Serial.begin(baud);
  Serial.print(F("\n\n"));
  Serial.print(FF(MSG_MAIN_TITLE));  
  Serial.print(F(" by sersad ["));
  Serial.print(__build_id);
  Serial.print(F("] Plugins registred: "));
  Serial.println(_pluginsCount);

  // Выводим на экран
  clear(); // Очищаем экран     
  moveCursor(2, 0);
  print(FF(MSG_MAIN_TITLE));       
  print(F("  ["));
  print(__build_id);
  print(F("]"));

  // Сбрасываем настройки EEPROM если надо
  if(_btn_analog2btn(_btn_read()) == DOWN) {
    if(_cfg.noValue) return; // Флаг уже не ноль. Сразу выходим, чтоб не циклиться
    _cfg.noValue = 0xff; //Ставим флаг, как будто во флеше ничего нету
    EEPROM.put(EEPROM_CFG_START, _cfg); //сохраняем значения   
    // сбрасываем флаги валидности для всех плагинов
    for(_mnu_current = 0; _mnu_current < _pluginsCount; _mnu_current++) EEPROM.update(_cfg_calc_offset() - 1, 0xff); // 0xff - значит данные невалидны. Адрес данных - 1 - там лежит флаг 
    Serial.print(FF(MSG_CLEAR_SETTINGS));
    Serial.print(F(" Rebooting..."));
    moveCursor(0, 1);
    println(FF(MSG_CLEAR_SETTINGS));
    delay(1000);
    asm volatile("jmp 0x00"); // soft reboot  
  }//if
}//init


void MultiSensCore::menu(){
  // Выводим текущий пункт
  moveCursor(0, 1);
  print(F("["));
  print(_mnu_current + 1);
  print(F("/"));
  print(_pluginsCount);
  print(F("] "));
  println(_plugins[_mnu_current].title);

  // Ждем команды
  MultiSensButton btn = wait4Button();
  
  switch(btn){
    case UP: 
    case UP_LONG: _mnu_current--; break;// Предыдущий пункт   
    
    case DOWN:
    case DOWN_LONG: _mnu_current++; break;// Следующий пункт
    
    case SELECT: _run_plugin(); break; // Запуск
           
    case SELECT_LONG: // Сохраняем текущий пункт меню и запускаем его 
        _cfg.mnu_current = _mnu_current;
        EEPROM.put(EEPROM_CFG_START, _cfg); //сохраняем значения   
        Serial.println(FF(MSG_SAVE_SETTINGS));
        moveCursor(0, 1);
        println(FF(MSG_SAVE_SETTINGS));        
        delay(1000);
        _run_plugin();
      break; // Сохранить выбор и запустить
    default: break;
  }//switch
  if(_mnu_current < 0 ) _mnu_current = _pluginsCount - 1;
  if(_mnu_current > _pluginsCount - 1) _mnu_current = 0;  
}//menu


void MultiSensCore::_run_plugin(){
  clear();// Чистим экран
  moveCursor((strlen(_plugins[_mnu_current].title) < 16)?(16 - strlen(_plugins[_mnu_current].title)) >> 1 : 0, 0);
  println(_plugins[_mnu_current].title); // Выводим названия плагина
  Serial.println(_plugins[_mnu_current].title);
  _plugins[_mnu_current].run(); // Запускаем плагин
  asm volatile("jmp 0x00");// Если вдруг плагин вышел (хотя не должен), перегружаемся как можем
}//_run_plugin



// == Строки ==
char * MultiSensCore::getPinName(uint8_t pinNumber){
  strcpy_P(_printBuf, MSG_PIN_NAME); // Скопировали базовый текст
  _printBuf[1] = '0' + (_ms_getP(pinNumber)); // Вписали номер пина
  return _printBuf; // Вернули указатель на буфер
}// getPinName


void MultiSensCore::printValScale(Print &p, int32_t value, int16_t scale){
  if(!value){
    p.print(0);
    return;
  }//if
  p.print(value / scale);
  p.print(F("."));
  p.print(abs(value % scale)); 
}//printValScale

  
char * MultiSensCore::rAlign(uint32_t val, const uint8_t width, const char fill, uint8_t base){
  return _rAlign(val, width, fill, base, 0);
}// rAlign
char * MultiSensCore::rAlign(int32_t val, const uint8_t width, const char fill, uint8_t base){
  if((base == 10) && (val < 0)) return _rAlign(-val, width, fill, base, 1); // Отрицательное число в десятичной системе
  return _rAlign(val, width, fill, base, 0); // Все остальное
}// rAlign

char * MultiSensCore::rAlign(uint16_t val, const uint8_t width, const char fill, uint8_t base){
  return rAlign((uint32_t) val, width, fill, base);
}// rAlign
char * MultiSensCore::rAlign(int16_t val, const uint8_t width, const char fill, uint8_t base){
  return rAlign((int32_t) val, width, fill, base);
}// rAlign

char * MultiSensCore::rAlign(uint8_t val, const uint8_t width, const char fill, uint8_t base){
  return rAlign((uint32_t) val, width, fill, base);
}// rAlign
char * MultiSensCore::rAlign(int8_t val, const uint8_t width, const char fill, uint8_t base){
  return rAlign((int32_t) val, width, fill, base);
}// rAlign


// == EEPROM ==
/*
 * Перед каждым блоком настроек плагина лежит байт флага(по смещению -1) валидный ли этот блок или был сброс.
 * При сбросе, проходим по цепочки и ставим эти флаги для всех плагинов.
 * Смещения, естественно, считаются с учетом этого "лишнего" байта.
*/
uint16_t MultiSensCore::_cfg_calc_offset(){
  uint16_t res = sizeof(_cfg) + 1; // Первый байт за базой - флаг валидности для первого плагина
  for(uint8_t i = 0; i < _mnu_current; i++) 
    res += (_plugins[i].cfg_size)?(_plugins[i].cfg_size + 1):0; // 1 байт - флаг валидности плагина
  res = min(res, 1023); // За границы EEPROM не выходим
  return res;
}//_cfg_calc_offset


void MultiSensCore::saveSettings(uint8_t * data){
  static uint32_t last_act = 0; // Первый запуск разрешать, а дальше чаще раза в секунду не делать
  if((millis() - last_act) < 1000) return; 
  last_act = millis();
  
  uint16_t start = _cfg_calc_offset(); // получили начальное смещение для текущего плагина
  if((start + _plugins[_mnu_current].cfg_size) > 1023) return; // За границы EEPROM-а не лезем
  EEPROM.update(start - 1, 0); // Ставим флаг валидности
  // Тут пишем
  for(uint8_t i = 0; i < _plugins[_mnu_current].cfg_size; i++) EEPROM.update(start + i,  data[i]); 
  
  // Даем сигнал, что записалось
  for(uint8_t i = LCD_PWM_VALUE; i > 0; i--){
    LCD_PWM_COUNTER_REG = i;
    delay(2);
  }//for
  for(uint8_t i = 0; i < LCD_PWM_VALUE; i++){
    LCD_PWM_COUNTER_REG = i;
    delay(2);
  }//for
}//saveSettings


bool MultiSensCore::loadSettings(uint8_t * data){
  uint16_t start = _cfg_calc_offset(); // получили начальное смещение
  //Читаем флаг валидности
  if(EEPROM.read(start - 1)) return false;  // Если там 0 - данные валидны, иначе нет.
  //Тут читаем
  for(uint8_t i = 0; i < _plugins[_mnu_current].cfg_size; i++) data[i] = EEPROM.read(start + i); 
  return true;
}//loadSettings



// == Экран ==
void MultiSensCore::setCursorType(MultiSensCursor ct){
 switch (ct)
 {
  case OFF:        _lcd_command(LCD_DM | LCD_DM_DISPLAY_ON | LCD_DM_LINE_CURSOR_OFF | LCD_DM_BLOCK_CURSOR_OFF); break;
  case UNDERLINE:  _lcd_command(LCD_DM | LCD_DM_DISPLAY_ON | LCD_DM_LINE_CURSOR_ON  | LCD_DM_BLOCK_CURSOR_OFF); break;
  case BLOCK:      _lcd_command(LCD_DM | LCD_DM_DISPLAY_ON | LCD_DM_LINE_CURSOR_OFF | LCD_DM_BLOCK_CURSOR_ON);  break; 
 }//switch  
 _lcd_cursor_type = ct;
}//setCursorType


void MultiSensCore::clear(){
  _lcd_command(LCD_CLEAR);
  _lcd_cursor_offset = 0;
  delayMicroseconds(2000);
}//lcdClear


void MultiSensCore::home(){
  _lcd_command(LCD_HOME);
  _lcd_cursor_offset = 0;
  delayMicroseconds(2000);
}//lcdClear


void MultiSensCore::moveCursor(uint8_t col, uint8_t row){
  // Проверки
  if(col > LCD_LINE_SIZE - 1) col = LCD_LINE_SIZE - 1;
  if(row > 1) row = 1;
  
  _lcd_cursor_offset = (LCD_LINE_OFFSET * row) + col; // Считаем смещение
  _lcd_command(LCD_AC_DDRAM | _lcd_cursor_offset);  // Записываем
}//lcdMoveCursor


void MultiSensCore::storeCursor(){
  _lcd_stored_cursor = _lcd_cursor_offset;
  _lcd_stored_cursor_type = _lcd_cursor_type;
}//lcdStoreCursor


void MultiSensCore::restoreCursor(){
  _lcd_cursor_offset = _lcd_stored_cursor; // Восстанавливаем
  _lcd_command(LCD_AC_DDRAM | _lcd_cursor_offset); // Записываем  
  setCursorType(_lcd_stored_cursor_type); // Восстанавливаем тип курсора 
}//lcdRestoreCursor


void MultiSensCore::createSymbol(const uint8_t code, MultiSensSymbol* data){
  _lcd_create_symbol(code & 0x01, data);//Разрешаем любой от 0 до 1 :)
}//crateSymbol



// == Кнопки ==
MultiSensButton MultiSensCore::getButton(){
  MultiSensButton result = buttonReleased();
  if(result != NONE) return result;
  static uint32_t dt = millis();
  if((_btn_pressed_code != NONE) && 
     (_btn_pressed_count > BTN_COUNT_LONG) &&
     ((millis() - dt) > BTN_LONG_REPEAT_MS)) {
       dt = millis();
       return (MultiSensButton)((uint8_t)_btn_pressed_code | MS_BTN_LONG_MASK);
     }//if
  return NONE;
}//getButton


MultiSensButton MultiSensCore::wait4Button(){
  MultiSensButton result;
  do {
    result = getButton();
  }while (result == NONE);
  return result;
}//wait4Button

  
MultiSensButton MultiSensCore::buttonPressed(){
  if(_btn_pressed_code == NONE) return NONE; // Если небыло нажато, то просто говорим не было

  // Длинное. Оставалось нажато более LONG циклов
  if(_btn_pressed_count > BTN_COUNT_LONG) return (MultiSensButton)((uint8_t)_btn_pressed_code | MS_BTN_LONG_MASK);

  //Короткое. Оставалось нажато более MIN циклов
  if(_btn_pressed_count > BTN_COUNT_MIN) return _btn_pressed_code;     
    
  return NONE; // Меньше MIN циклов
}//buttonPressed


MultiSensButton MultiSensCore::buttonReleased(){
  MultiSensButton result = NONE;  

  switch(_btn_released_code){
    case NONE: return NONE; // Если пусто, то пусто
    
    case LEFT:
    case RIGHT:
    case LEFT_LONG:
    case RIGHT_LONG: break; // Эти кнопки - скроллинг. Их не отдаем
    
    default:
            if(_btn_released_count > BTN_COUNT_MIN) result = _btn_released_code; // На короткое нажатие наработали
            if(_btn_released_count > BTN_COUNT_LONG) result = (MultiSensButton)((uint8_t)_btn_released_code | MS_BTN_LONG_MASK); // На длинное нажатие наработали     
            break;
  }//switch

  // Сбрасываем
  _btn_released_code = NONE; 
  _btn_released_count = 0;
    
  return result; // Отдаем результат
}//buttonReleased



// == Внутренности ==
char * MultiSensCore::_rAlign(uint32_t val, uint8_t width, const char fill, uint8_t base, uint8_t isNegative){
  char *ptr = &_printBuf[sizeof(_printBuf) - 1]; // Указатель на конец строки
  uint8_t size = 0; // Текущаа длина строки
  *ptr = '\0'; // Ставим окончание строки
  if((base < 2) || (isNegative)) base = 10; // Основание не может быть 0 или 1. Для отрицательных чисел основание всегда 10 
  width = constrain(width, 1, arraySize(_printBuf) - 1); // В диапазоне 1..размер буфера
  do{
    char c = val % base; //Младшая цифра
    val /= base; // Сместились на один разряд
    *--ptr = (c < 10) ? ('0' + c) : ('A' + c - 10); //Хватит только цифр или нужны еше буквы A-F 
    size++; 
  } while(val);
  if((isNegative) && (fill == ' ')){ // Для заполнения пробелами сразу ставим минус
    *--ptr = '-'; 
    size++;
    isNegative = 0; // Больше минусов не надо
  }//if
  for(uint8_t i = size; i < width; i++) *--ptr = fill; // Заполнение нужными символами
  if(isNegative) *ptr = '-'; // Ставим минус вместо первого символа
  if(size > width) ptr += (size - width); // Больше, чем width не выдавать. Обрезаем.
  return ptr;
}//_rAlign


void MultiSensCore::_btn_isr(){
  MultiSensButton b = _btn_analog2btn(_btn_read()); // Читаем кнопку
  if(_btn_pressed_code != b) { // Новая кнопка
    _btn_released_code = _btn_pressed_code; // Ту кнопку отпустили
    _btn_released_count = _btn_pressed_count; // И продержалась она вот столько    
    _btn_pressed_code = b;
    _btn_pressed_count = 0;
  }//if

  _btn_pressed_count = min(_btn_pressed_count + 1, BTN_COUNT_MAX); // Что б не переполнялось, если слишком долго держат

  // Скролл экрана
  switch(buttonPressed()){
    case RIGHT_LONG: 
    case RIGHT: _lcd_scroll_left();  break;
    case LEFT: _lcd_scroll_right(); break;
    case LEFT_LONG: storeCursor(); home(); restoreCursor(); break;
    default: break;
  }//switch  
}//checkButton


int MultiSensCore::_btn_read(){
  //Опорное напряжение = VCC плюс номер пина (младшие 3 бита от 0 до 7)
  ADMUX = (1 << 6) | (BTN_PIN & 0x07); 

  //Запускаем АЦП
  _INT_OFF;
  _PORT_SET_HIGH(ADCSRA, _BV(ADSC)); 
  _INT_ON;

  //Ждем окончания преобразования
  while(_PORT_READ(ADCSRA, _BV(ADSC)));
  
  //Возвращаем результат (важно! сначало читаем младшие биты)
  return (ADCL | (ADCH << 8));
}//_btn_read


MultiSensButton MultiSensCore::_btn_analog2btn(int code){
  switch (code){
    case   0 ... 99 : return RIGHT; 
    case 100 ... 199: return UP; 
    case 200 ... 399: return DOWN; 
    case 400 ... 599: return LEFT; 
    case 600 ... 799: return SELECT; 
    default: return NONE;
  }//switch
}//_bnt_analog2btn


void MultiSensCore::_lcd_create_symbol(const uint8_t code, MultiSensSymbol* data){
  _lcd_command(LCD_AC_CGRAM | (code << 3));
  for(uint8_t i = 0; i < 8; i++){
    _lcd_data(pgm_read_byte(&data[i])); 
  }  
}//_lcd_create_symbol


void MultiSensCore::_lcd_scroll_left(){
  _lcd_command(LCD_CSM | LCD_CSM_SCREEN | LCD_CSM_L);
}//_lcd_scroll_left


void MultiSensCore::_lcd_scroll_right(){
  _lcd_command(LCD_CSM | LCD_CSM_SCREEN | LCD_CSM_R);
}//_lcd_scroll_right


// Перекрываем write от принта. Теперь можно вызывать print/println
size_t MultiSensCore::write(uint8_t value){  
   // Проверка на перевод строки
   if(value == 0x0d) return 1; // CR - вообще не печатаем
   if(value == 0x0a){ // По LF - забиваем текущую строку пробелами до конца
     if(_lcd_cursor_offset >= LCD_LINE_OFFSET) _lcd_cursor_offset -= LCD_LINE_OFFSET; // Это вторая строка
     for(uint8_t i = _lcd_cursor_offset; i < LCD_LINE_SIZE; i++) _lcd_data(0x20); // Забиваем пробелами
     _lcd_cursor_offset = LCD_LINE_OFFSET; // Курсор на начало второй строки
     _lcd_command(LCD_AC_DDRAM | _lcd_cursor_offset);
     return 1;
   }//
   
   // Проверки на выход за границы
  _lcd_cursor_offset ++; // Увеличиваем счетчик курсора
  if((_lcd_cursor_offset > LCD_LINE_SIZE) && (_lcd_cursor_offset < LCD_LINE_OFFSET)) return 1; // Ужа за первой, но еще не на второй строке
  if(_lcd_cursor_offset > LCD_LINE_OFFSET + LCD_LINE_SIZE) return 1; // За пределами второй строки 
  
  _lcd_data(value); // Отправляем данные  
  return 1;
}//wtite


void MultiSensCore::_lcd_command(uint8_t value){
  _PORT_SET_LOW(LCD_CTRL_PORT, _BV(LCD_RS_PIN)); // RS - LOW
  _lcd_send_byte(value); 
}//_lcd_data


void MultiSensCore::_lcd_data(uint8_t value){
  _PORT_SET_HIGH(LCD_CTRL_PORT, _BV(LCD_RS_PIN)); // RS - HIGH
  _lcd_send_byte(value); 
}//_lcd_data


void MultiSensCore::_lcd_send_byte(uint8_t value){
  _lcd_send_half_byte(value >> 4); //Старший полубайт
  _lcd_send_half_byte(value); //Младший полубайт
}//_lcd_send_byte


void MultiSensCore::_lcd_send_half_byte(uint8_t value){
  LCD_DATA_PORT = (LCD_DATA_PORT & ~(LCD_DATA_MASK)) | ((value << 4) & LCD_DATA_MASK);// Выводим младший полубайт
  //Дергаем шину
  _PORT_SET_LOW(LCD_CTRL_PORT, _BV(LCD_EN_PIN)); 
  delayMicroseconds(1); 
  _PORT_SET_HIGH(LCD_CTRL_PORT, _BV(LCD_EN_PIN)); 
  delayMicroseconds(1); 
  _PORT_SET_LOW(LCD_CTRL_PORT, _BV(LCD_EN_PIN)); 
  delayMicroseconds(50);
}//_lcd_send_half_byte


MultiSensCore core = MultiSensCore(); // Глобальное определение core как экземпляра MultiSensCore

// Обработчик прерывания для опроса кнопки
// По пререполнению. По сравнению одновременно с PWM не работает. В результате 490 раз в секунду.
ISR(TIMER1_OVF_vect){
  static uint8_t cnt = 0;
  if(++cnt < 49) return; // 49 проходов до вызова дают обработку кнопки 10 раз в секунду
  cnt = 0;
  core._btn_isr();
}//ISR
