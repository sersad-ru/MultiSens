#pragma once

// Манипуляции с портами
#define _PORT_OUTPUT(P, M)   (P |=  (M))           // Сконфигурировать порт на вывод
#define _PORT_INPUT(P, M)    (P &= ~(M))           // Сконфигурировать порт на ввод
#define _PORT_SET_LOW(P, M)  (P &= ~(M))           // Сбросить пин
#define _PORT_SET_HIGH(P, M) (P |=  (M))           // Установить пин
#define _PORT_READ(P, M)    ((P &   (M)) ? 1 : 0)  // Прочесть пин

// Макросы и переменная для сохранения статусного регистра и запрета/разрешения прерываний
#define _INT_OFF _mscore_oldSREG = SREG; cli()
#define _INT_ON SREG = _mscore_oldSREG
uint8_t _mscore_oldSREG;
