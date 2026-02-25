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
