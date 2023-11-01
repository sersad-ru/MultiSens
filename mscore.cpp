#include <Arduino.h>
#include "mscore.h"
#include "buildid.h"


// Порт и пины Дисплея
#define LCD_DATA_PORT PORTD // Полупорт обмена данными D0-D4 (Старший полупорт D: PD4-PD7, пины 4-7)
#define LCD_DATA_PORT_CFG DDRD // Конфигурация порта данных
#define LCD_DATA_MASK 0xF0 // У порта используются старшие биты
#define LCD_CTRL_PORT PORTB // Порт пинов управления RS, EN, BL (PB0-PB2, пины 8-10)
#define LCD_CTRL_PORT_CFG DDRB // Конфигурация порта управления
#define LCD_RS_PIN 0 // Пин RS (PB0, пин 8)
#define LCD_EN_PIN 1 // Пин EN (PB1, пин 9)
#define LCD_BL_PIN 2 // Пин BL (PB2, пин 10)


//Управление направлением сдвига курсора или экрана HD_IR_CMDS | HD_IR_SCMS_ID_ON | HD_IR_CMDS_S_OFF
#define LCD_CMDS     0x04 // Sets cursor move direction and specifies display shift. These operations are performed during data write and read.  
#define LCD_CMDS_INC 0x02 // Увеличивать позицию курсора или сдвигать экран влево
#define LCD_CMDS_DEC 0x00 // Уменьшать позицию курсова или сдвигать экран вправо
#define LCD_CMDS_SCR 0x01 // ID - управляет направлением сдвига экрана
#define LCD_CMDS_CUR 0x00 // ID - управляет направлением сдвига курсора

//Управление параметрами экрана и шириной ШД
#define LCD_F       0x20 // Sets interface data length (DL), number of display lines (N), and character font (F).
#define LCD_F_8BIT  0x10 // Шина 8 бит
#define LCD_F_4BIT  0x00 // Шина 4 бита
#define LCD_F_2LINE 0x08 // Развертка 2 строки
#define LCD_F_1LINE 0x00 // Развертка 1 строкой
#define LCD_F_5x10  0x04 // Шрифт 5x10
#define LCD_F_5x8   0x00 // Шрифт 5x8


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

// Макрос дерганья шины
#define LCD_PULSE _PORT_SET_LOW(LCD_CTRL_PORT, _BV(LCD_EN_PIN)); delayMicroseconds(1); _PORT_SET_HIGH(LCD_CTRL_PORT, _BV(LCD_EN_PIN)); delayMicroseconds(1); _PORT_SET_LOW(LCD_CTRL_PORT, _BV(LCD_EN_PIN)); delayMicroseconds(50)

MultiSensCore::MultiSensCore(){
  _pluginsCount = 0;

}//constructor

//init
void MultiSensCore::init(MultiSensPlugin* plugins, uint8_t pluginsCount, uint32_t baud){
  _plugins = plugins;
  _pluginsCount = pluginsCount;
  
  // Инициализируем дисплей
  // Сначала порты
  _INT_OFF;
  _PORT_OUTPUT(LCD_CTRL_PORT_CFG, _BV(LCD_RS_PIN) | _BV(LCD_EN_PIN));// | _BV(LCD_BL_PIN) - добавить, а потом поднять этот пин?
  _PORT_OUTPUT(LCD_DATA_PORT_CFG, LCD_DATA_MASK);
  _INT_ON;

  // Ждем 15мс
  delayMicroseconds(15000); // Больше 16383 - функция косячит (https://www.arduino.cc/reference/en/language/functions/time/delaymicroseconds/)
  
  // RS низким
  _PORT_SET_LOW(LCD_CTRL_PORT, _BV(LCD_RS_PIN));  
  _lcd_send_half_byte(0x03); // Первая отправка команды инициализация 4-х битного режима
  delayMicroseconds(4500); // Ждем 4.1 ms
  _lcd_send_half_byte(0x03); // Вторая отправка команды инициализация 4-х битного режима
  delayMicroseconds(100); //Ждем 100 ms
  _lcd_send_half_byte(0x03); // Третья отправка команды инициализация 4-х битного режима
  delayMicroseconds(150); // Ждем 150
  _lcd_send_half_byte(0x02); // Первя отправка только 
  _lcd_command(LCD_F | LCD_F_4BIT | LCD_F_2LINE | LCD_F_5x8); // Остальные настройки
  _lcd_command(LCD_CMDS | LCD_CMDS_INC | LCD_CMDS_CUR);//Включить экран без курсора

 //!!!!!!!!!!!!!!!!!!!!!1 Дописать инициализацию и остальную работу с дисплеем
  
  // Выводим стартовое сообщение
  Serial.begin(baud);
  Serial.print(F("\n\nMultiSens by sersad ("));
  Serial.print(__build_id);
  Serial.print(F(") Plugins registred: "));
  Serial.println(_pluginsCount);

/*  
  uint32_t t = micros();
  delayMicroseconds(15000);
  delayMicroseconds(15000);
  delayMicroseconds(15000);
  t = micros() - t;
  Serial.print("T: "); 
  Serial.println(t);
*/
}//init


void MultiSensCore::_lcd_command(uint8_t value){
  _PORT_SET_LOW(LCD_CTRL_PORT, _BV(LCD_RS_PIN)); // RS - HIGH
  _lcd_send_half_byte(value >> 4); //Старший полубайт
  _lcd_send_half_byte(value); //Младший полубайт  
//  _lcd_send_byte(value); 
}//_lcd_data


void MultiSensCore::_lcd_data(uint8_t value){
  _PORT_SET_HIGH(LCD_CTRL_PORT, _BV(LCD_RS_PIN)); // RS - HIGH
  _lcd_send_half_byte(value >> 4); //Старший полубайт
  _lcd_send_half_byte(value); //Младший полубайт
//  _lcd_send_byte(value); 
}//_lcd_data

/*
void MultiSensCore::_lcd_send_byte(uint8_t value){
  LCD_DATA_PORT = (LCD_DATA_PORT & ~(LCD_DATA_MASK)) | (value & LCD_DATA_MASK);// Выводим старший полубайт 
  LCD_PULSE; //Дергаем шину
  LCD_DATA_PORT = (LCD_DATA_PORT & ~(LCD_DATA_MASK)) | ((value << 4) & LCD_DATA_MASK);// Выводим младший полубайт
  LCD_PULSE; //Дергаем шину
}//_lcd_send_byte
*/

void MultiSensCore::_lcd_send_half_byte(uint8_t value){
  LCD_DATA_PORT = (LCD_DATA_PORT & ~(LCD_DATA_MASK)) | ((value << 4) & LCD_DATA_MASK);// Выводим младший полубайт
  LCD_PULSE; //Дергаем шину
}//_lcd_send_byte
