#pragma once
#include <Arduino.h>
/*
* Micro Frequency Detector
* 25.04.2025
*
* This library is based on Arduino-FrequencyDetector library by Armin Joachimsmeyer
* https://github.com/ArminJo/Arduino-FrequencyDetector
*
*/

// Полный диапазон частот
#define uFD_FREQ_MIN 625
#define uFD_FREQ_MAX 9612

// Ошибки определения частоты
#define uFD_ERR_NO_TRIGGER    0  // Триггеров не найдено
#define uFD_ERR_SIGNAL_LOW    1  // Сигнал слишком слабый
#define uFD_ERR_FREQ_LOW      2  // Частота слишком низкая
#define uFD_ERR_FREQ_HIGH     3  // Частота слишком высокая
#define uFD_ERR_EDISTR_FAILED 4  // Слишком много ошибок вылета за равномрное опередление
#define uFD_ERR_MAX_ERROR     4  // Максимальный код ошибки

//** Получить частоту и громкость звука. Возвращает частоту сигнала или ошибку определения частоты (0..uFD_ERR_MAX_ERROR)
//* pin - Аналоговый пин, к которому подключен микрофон (A0..A7)
//* vol - Сюда вернется значение громкости (0..1024)
uint16_t uFD_getFreq(const uint8_t pin, uint16_t &vol);


//** Получить частоту и громкость звука. Возвращает частоту сигнала или ошибку определения частоты (0..uFD_ERR_MAX_ERROR)
//* pin - Аналоговый пин, к которому подключен микрофон (A0..A7)
uint16_t uFD_getFreq(const uint8_t pin = A0);


//** Проверить попадает ли найденная частота в диапазон. Вернет частоту, если попадает и 0 - если нет (или были ошибки)
//* freq - найденна частота
//* freqMin - минимальная частота диапазона
//* freqMax - максимальная частота диапазона
uint16_t uFD_freqMatch(const uint16_t freq, const uint16_t freqMin = uFD_FREQ_MIN, const uint16_t freqMax = uFD_FREQ_MAX);