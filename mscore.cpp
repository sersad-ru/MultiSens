#include "mscore.h"
#include "msdisp.h"
#include "buildid.h"

#include <Arduino.h>


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
#define BTN_COUNT_MIN 1    // Минимальное количество вывовов таймера, чтобы засчитать нажатие (0,1 секунды)
#define BTN_COUNT_LONG 10  // Количество вызовов таймера, чтобы засчитать длинное нажатие (1 секунда)
#define BTN_COUNT_MAX 250  // Максимальное количество вызовов (что б счетчик через 0 не переходил)


// Манипуляции с портами
#define _PORT_OUTPUT(P, M)   (P |=  (M))           // Сконфигурировать порт на вывод
#define _PORT_INPUT(P, M)    (P &= ~(M))           // Сконфигурировать порт на ввод
#define _PORT_SET_LOW(P, M)  (P &= ~(M))           // Сбросить пин
#define _PORT_SET_HIGH(P, M) (P |=  (M))           // Установить пин
#define _PORT_READ(P, M)     ((P &   (M)) ? 1 : 0) // Прочесть пин

// Макросы и переменная для сохранения статусного регистра и запрета/разрешения прерываний
#define _INT_OFF _mscore_oldSREG = SREG; cli()
#define _INT_ON SREG = _mscore_oldSREG
uint8_t _mscore_oldSREG;


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
  _PORT_SET_LOW(LCD_CTRL_PORT, _BV(LCD_RS_PIN)); // RS низким 
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
  _lcd_create_symbol(MS_SYM_BACKSLASH_CODE, MS_SYM_BACKSLASH);// Символ обратной косой черты
  _lcd_create_symbol(MS_SYM_RESET_CODE, MS_SYM_RESET);// Символ сброса
  _lcd_create_symbol(MS_SYM_SELECT_CODE, MS_SYM_SELECT);// Символ выбора
  _lcd_create_symbol(MS_SYM_ARROW_UPDOWN_CODE, MS_SYM_ARROW_UPDOWN);// Символ стрелки вверх-вниз
  _lcd_create_symbol(MS_SYM_ARROW_DOWN_CODE, MS_SYM_ARROW_DOWN);// Символ стрелки вниз
  _lcd_create_symbol(MS_SYM_ARROW_UP_CODE, MS_SYM_ARROW_UP);// Символ стрелки вверх
  
  clear(); // Очищаем экран   
   
  // Выводим стартовое сообщение
  Serial.begin(baud);
  Serial.print(F("\n\nMultiSens by sersad ["));
  Serial.print(__build_id);
  Serial.print(F("] Plugins registred: "));
  Serial.println(_pluginsCount);
}//init


void MultiSensCore::setCursorType(MultiSensCursor ct){
 switch (ct)
 {
  case OFF:        _lcd_command(LCD_DM | LCD_DM_DISPLAY_ON | LCD_DM_LINE_CURSOR_OFF | LCD_DM_BLOCK_CURSOR_OFF); break;
  case UNDERLINE:  _lcd_command(LCD_DM | LCD_DM_DISPLAY_ON | LCD_DM_LINE_CURSOR_ON  | LCD_DM_BLOCK_CURSOR_OFF); break;
  case BLOCK:      _lcd_command(LCD_DM | LCD_DM_DISPLAY_ON | LCD_DM_LINE_CURSOR_OFF | LCD_DM_BLOCK_CURSOR_ON);  break; 
 }//switch  
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
}//lcdStoreCursor


void MultiSensCore::restoreCursor(){
  _lcd_cursor_offset = _lcd_stored_cursor; // Восстанавливаем
  _lcd_command(LCD_AC_DDRAM | _lcd_cursor_offset); // Записываем  
}//lcdRestoreCursor


void MultiSensCore::createSymbol(const uint8_t code, MultiSensSymbol* data){
  _lcd_create_symbol(code & 0x01, data);//Разрешаем любой от 0 до 1
}//crateSymbol


MultiSensButton MultiSensCore::getButton(){
  MultiSensButton result;
  do {
    result = buttonReleased();
  }while (result == NONE);
  return result;
}//getButton

  
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



void MultiSensCore::_btn_isr(){
  MultiSensButton b = _btn_analog2btn(_btn_read()); // Читаем кнопку
  if(_btn_pressed_code != b) { // Новая кнопка
    _btn_released_code = _btn_pressed_code; // Ту кнопку отпустили
    _btn_released_count = _btn_pressed_count; // И продержалась она вот столько    
    _btn_pressed_code = b;
    _btn_pressed_count = 0;
  }//if

  _btn_pressed_count = min(_btn_pressed_count + 1, BTN_COUNT_MAX);

  // Скролл экрана
  switch(buttonPressed()){
    case LEFT_LONG:
    case LEFT: _lcd_scroll_left();  break;
    case RIGHT: _lcd_scroll_right(); break;
    case RIGHT_LONG: home(); break;
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
  
  //Возвращаем результат (важно! сначало читаем младший бит)
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
  _PORT_SET_LOW(LCD_CTRL_PORT, _BV(LCD_RS_PIN)); // RS - HIGH
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
}//_lcd_send_byte


MultiSensCore core = MultiSensCore();

// Обработчик прерывания для опроса кнопки
// По пререполнению. По сравнению одновременно с PWM не работает. В результате 490 раз в секунду.
ISR(TIMER1_OVF_vect){
  static uint8_t cnt = 0;
  if(++cnt < 49) return; // 49 проходов до вызова дают обработку кнопки 10 раз в секунду
  cnt = 0;
  core._btn_isr();
}//ISR
