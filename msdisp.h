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

/*
* Константы упраления экраном HD44780
*/

// Инициализация режима 4-х битной шины
#define LCD_INIT_4BIT 0x03

// Длина строки 40 символов (лишние за границей экрана и работают для скроллинга)
#define LCD_LINE_SIZE 40

// Смещение второй строки в буфере (вторая строка адресуется с 0x40 (64), а не с 21-го или 41-го символа)
#define LCD_LINE_OFFSET 0x40

//Очистка экрана
#define LCD_CLEAR 0x01 // Очистить экран и поместить курсор в само начало

//Перемещение курсора в начало (левый вержхний угол) и отмена сдвигов
#define LCD_HOME  0x02 // Курсор в начало. Сдвиги отменить

// Управление направлением сдвига курсора или экрана HD_IR_CMDS | HD_IR_SCMS_ID_ON | HD_IR_CMDS_S_OFF
#define LCD_CMDS     0x04 // Установка направления сдвига курсора и прокрутки экрана
#define LCD_CMDS_INC 0x02 // Увеличивать позицию курсора или сдвигать экран влево
#define LCD_CMDS_DEC 0x00 // Уменьшать позицию курсова или сдвигать экран вправо
#define LCD_CMDS_SCR 0x01 // ID - управляет направлением сдвига экрана
#define LCD_CMDS_CUR 0x00 // ID - управляет направлением сдвига курсора

//Управление режимом отображения
#define LCD_DM                  0x08 // Включение/выключение экрана, курсора и мигания курсора
#define LCD_DM_DISPLAY_ON       0x04 // Включить отображение
#define LCD_DM_DISPLAY_OFF      0x00 // Выключить отображение
#define LCD_DM_LINE_CURSOR_ON   0x02 // Включить курсор в виде подчерка
#define LCD_DM_LINE_CURSOR_OFF  0x00 // Выключить курсор в виде подчерка
#define LCD_DM_BLOCK_CURSOR_ON  0x01 // Включить курсор в виде блока
#define LCD_DM_BLOCK_CURSOR_OFF 0x00 // Выключить курсор в виде блока

//Сдвинуть курсор или экран
#define LCD_CSM        0x10 // Сдвинуть курсор или экран
#define LCD_CSM_SCREEN 0x08 // Сдвигать экран
#define LCD_CSM_CURSOR 0x00 // Сдвигать курсор
#define LCD_CSM_R      0x04 // Сдвигать вправо
#define LCD_CSM_L      0x00 // Сдвигать влево

// Управление параметрами экрана и шириной ШД
#define LCD_F       0x20 // Настройки шины, количества строк и шрифта
#define LCD_F_8BIT  0x10 // Шина 8 бит
#define LCD_F_4BIT  0x00 // Шина 4 бита
#define LCD_F_2LINE 0x08 // Развертка 2 строки
#define LCD_F_1LINE 0x00 // Развертка 1 строкой
#define LCD_F_5x10  0x04 // Шрифт 5x10
#define LCD_F_5x8   0x00 // Шрифт 5x8

//Установка значения AC в области CGRAM
#define LCD_AC_CGRAM 0x40 // Установить CGRAM адрес

//Установка значения AC в области DDRAM
#define LCD_AC_DDRAM 0x80 //Установить DDRAM адрес

//Флаг занятости
#define LCD_BUSY 0x80 // Флаг занятости контроллера
