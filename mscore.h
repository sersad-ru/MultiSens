#pragma once
#include <inttypes.h>

class MultiSensCore;

// Plugin functione type
typedef void (*PluginFunction)(MultiSensCore& core);

// Plugin element
typedef struct {
  PluginFunction run; //Plugin function
  const char* Title; //Plugin title
} MultiSensPlugin;

class MultiSensCore {
  public:
  //** Constructor
  MultiSensCore();

  //** Initialization
  //* baud - Baudrate for serial port logs
  void init(MultiSensPlugin* plugins, uint8_t pluginsCount, uint32_t baud = 9600);
  
  private:
  MultiSensPlugin * _plugins; // Массив плагинов 
  uint8_t _pluginsCount; // Количество элементов массива плагинов

  void _lcd_command(uint8_t value); // Отправить команду дисплею
  void _lcd_data(uint8_t value); // Отправить данные дисплею
  //void _lcd_send_byte(uint8_t value); // Отправить байт на шину данных дисплея
  void _lcd_send_half_byte(uint8_t value); //Отправить полбайта (младшие) на шину данных дисплея
}; //class
