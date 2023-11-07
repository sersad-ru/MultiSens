#pragma once

// Конфигурация, хранимая во флеше
typedef struct {
  uint8_t noValue; //Флаг того (==255), что не было сохраненных значений 
  int8_t mnu_current; // Текущий активный пункт меню  
} flashcfg;

