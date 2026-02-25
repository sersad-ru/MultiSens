#include "uFD.h"

// Опорное напряжение АЦП (DEFAULT = 1 или INTERNAL = 0)
#define uFD_ADC_REFERENCE DEFAULT

// Делитель таймера (1024 выборки 16 MHz prescaler = 4:  13.312 ms / 75.12 Hz. 13 циклов/сэмпл =>  13 us/sample | 76923 Hz sample rate)
#define uFD_PRESCALER 4

// Минимальный уровень сигнала (1/16 от максимума: 1024 / 16 = 64 (0x40)) 
#define uFD_MIN_DELTA 64

// Количество сэмплов
#define uFD_SAMPLES 1024

// Размер массива для сэмплов (8 сэмплов на период)
#define uFD_ARRAY_SIZE (uFD_SAMPLES >> 3) // 128

// Время на получения одного сэмпла в мкс
#define uFD_SAMPLE_US 13

// Значение для коррекции счетчика millis в мс
#define uFD_MILLIS_CORRECTION_MS ((uFD_SAMPLE_US * uFD_SAMPLES) / 1000)

// Минимальное необходимое количество триггеров в буфере 8
#define uFD_MIN_TRIGGRES 8

// Максимальное время, когда идут ошибки (мс)
#define uFD_MAX_DROP_MS 200
#define uFD_MAX_DROP ((uFD_MAX_DROP_MS * 1000L) / ((uFD_SAMPLE_US * uFD_SAMPLES) + (625 / (F_CPU / 1000000L)))) // 625 - примерно от фонаря. Количество тактов на получение сигнала кроме самого цикла

// Минимальное время без ошибок (мс)
#define uFD_MIN_NO_DROP_MS 150
#define uFD_MIN_DROP ((uFD_MIN_NO_DROP_MS * 1000L) / ((uFD_SAMPLE_US * uFD_SAMPLES) + (625 / (F_CPU / 1000000L))))

// Фильтры
#define uFD_FILTER_MAX 200
#define uFD_FILTER_MIN 0
#define uFD_FILTER_MATCH ((uFD_FILTER_MAX + uFD_FILTER_MIN) >> 1) // (Max + Min) / 2
#define uFD_FILTER_HIGHER (uFD_FILTER_MAX - (uFD_FILTER_MATCH >> 1))
#define uFD_FILTER_LOWER  (uFD_FILTER_MIN + (uFD_FILTER_MATCH >> 1))


// Побайтовая работа со словом
union uFD_splitWord {
  struct {
    uint8_t Low;
    uint8_t High;
  } u8;
  uint16_t u16;
};

#if !defined(cbi)
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#if !defined(sbi)
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

extern volatile unsigned long timer0_millis; // Доступ к переменной, хранящей millis


//** Получить частоту и громкость звука. Возвращает частоту сигнала или ошибку определения частоты (0..uFD_ERR_MAX_ERROR)
//* pin - Аналоговый пин, к которому подключен микрофон (A0..A7)
//* vol - Сюда вернется значение громкости (0..1024)
uint16_t uFD_getFreq(const uint8_t pin, uint16_t &delta){
  uint8_t oldADMUX = ADMUX; // Сохраняем старое состояние регистра
  uFD_splitWord val; // Значение с АЦП

  uint8_t toHigh = false; // Флаг слишком высокой частоты

  uint16_t valMax = 0; // Максимальное полученное значение
  uint16_t valMin = 1024; // Минимальное полученное значение
  
  uint8_t firstFound = false; // Найден ли уже первый триггер
  uint8_t searchStart = true; // Флаг начала поиска
  uint8_t periods[uFD_ARRAY_SIZE]; // Массив длительностей периодов (есть шанс переполнения?)
  uint8_t pCnt = 0; // Счетчик периодов

  uint16_t firstPos = 0; // Положение первого триггера
  uint16_t lastPos = 0; // Положение последнего триггера

  static uint16_t tLevel = 0; // Середина между минимумом и максимумом valMin + ((valMax - valMin) / 2)
  static uint16_t tLower = 0; // Середина - гистерезис (разброс значений (valMax - valMin) / 8)

  cbi(TIMSK0, TOIE0); // Запрещаем прерывания таймера 0.

  // Настраиваем АЦП на наш пин
  ADMUX = (uFD_ADC_REFERENCE << REFS0) | (pin - 14); // 0..7 <- A0 = 14, A7 = 21
  ADCSRA = ((1 << ADEN) | (1 << ADSC) | (1 << ADATE) | (1 << ADIF) | uFD_PRESCALER);

  // Читаем сэмплы
  for(uint16_t i = 0; i < uFD_SAMPLES; i++){
    loop_until_bit_is_set(ADCSRA, ADIF); // Ждем данных от АЦП

    // получаем значение
    val.u8.Low = ADCL;
    val.u8.High = ADCH;
    ADCSRA |= (1 << ADIF); // Сбрасываем бит, для следующего loop_until..

    // Собираем max-min
    valMin = min(valMin, val.u16);
    valMax = max(valMax, val.u16);
  
    // Триггеры
    if(searchStart){
      searchStart = (val.u16 >= tLower); // Пока значение < tLower, serachStart будет true и крутимся в этой точке
      continue; // 
    }//if
    if(val.u16 <= tLevel) continue; // Ждем пока не поднялось до tLevel крутимся тут
    // Нашли триггер
    if(firstFound) { // Первый уже был найден
      if(pCnt >= uFD_ARRAY_SIZE - 1) toHigh = true; // Выскочили за пределы массива. Значит частота слишком высока
        else periods[pCnt] = i - lastPos; // Сохранили длительность периода (тут не выскочим за uint8_t?)
      pCnt++;
    } else firstPos = i; // Первого еще не было. Сохранили позицию первого.
    lastPos = i; // позиция последнего триггера
    firstFound = true; // Первый уже был найден
    searchStart = true;
  }//for

  // Разрешаем прерывания таймера 0 и компенсируем упущенные микросекунды
  if ((TIMSK0 & _BV(TOIE0)) == 0) timer0_millis += uFD_MILLIS_CORRECTION_MS;
  sbi(TIMSK0, TOIE0);

  // Восстанавливаем настройки АЦП
  ADCSRA &= ~(1 << ADATE);
  ADMUX = oldADMUX;

  // Пошла постобработка
  delta = valMax - valMin; // Разброс значений
  tLevel = valMin + (delta >> 1); // valMin + (max - min) / 2

  if(toHigh) return uFD_ERR_FREQ_HIGH; // Если уже определили, что частота слишком высокая, то дальше делать нечего

  // Проверяем силу сигнала
  if(delta < uFD_MIN_DELTA) return uFD_ERR_SIGNAL_LOW; // Сигнал слишком слабый

  // Гистерезис
  tLower = tLevel - (delta >> 3); // (max - min) / 8

  if(!pCnt) return uFD_ERR_NO_TRIGGER; // Не нашли ни одного триггера
  if(pCnt < uFD_MIN_TRIGGRES) return uFD_ERR_FREQ_LOW; // Частота слишком низкая

  // Считаем ошибки выхода за пределы равномерного распределения
  uint8_t averagePeriod = (lastPos - firstPos) / pCnt;
  uint8_t periodMax = averagePeriod + (averagePeriod >> 1); // /2
  uint8_t periodMin = averagePeriod - (averagePeriod >> 2); // /4
  uint8_t errCnt = 0;
  for(uint8_t i = 0; i < pCnt; i++)
    if((periods[i] > periodMax) || (periods[i] < periodMin)) errCnt++;

  if(errCnt > (pCnt >> 3)) return uFD_ERR_EDISTR_FAILED; // Ошибок больше, чем pCnt / 8

  return ((uint32_t)pCnt * (1000000L / uFD_SAMPLE_US)) / (lastPos - firstPos); // Вернули частоту 
}//uFD_getFreq


//** Получить частоту и громкость звука. Возвращает частоту сигнала или ошибку определения частоты (0..uFD_ERR_MAX_ERROR)
//* pin - Аналоговый пин, к которому подключен микрофон (A0..A7)
uint16_t uFD_getFreq(const uint8_t pin){
  uint16_t vol;
  return uFD_getFreq(pin, vol);
}//uFD_getFreq


uint16_t uFD_do_lpf(int16_t fVal, int16_t val){
  return (fVal + (((val - fVal) + (1 << 3)) >> 4));
}//_do_lpf


//** Проверить попадает ли найденная частота в диапазон. Вернет частоту, если попадает и 0 - если нет (или были ошибки)
//* freq - найденна частота
//* freqMin - минимальная частота диапазона
//* freqMax - максимальная частота диапазона
uint16_t uFD_freqMatch(const uint16_t freq, const uint16_t freqMin, const uint16_t freqMax){
  static uint8_t dropCount = uFD_MAX_DROP + uFD_MIN_DROP;

  if(freq <= uFD_ERR_MAX_ERROR){ // Частота определена с ошибкой
    dropCount++;
    dropCount = min(dropCount, (uFD_MAX_DROP + uFD_MIN_DROP)); // За диапазон не вылезаем
    return 0; 
  }//if 

  // Ошибки не было
  if(dropCount > 0) dropCount--; // Уменьшаем дропы до 0

  // Ставим фильтры
  uint8_t filter = uFD_FILTER_MATCH;
  if(freq < freqMin) filter = uFD_FILTER_MIN;
  if(freq > freqMax) filter = uFD_FILTER_MAX;
  
  static uint8_t lpf = 0;
  uint16_t filtered = 0;
  if(dropCount == uFD_MAX_DROP) {
    lpf = filter;
    filtered = freq;
  } else if (dropCount < uFD_MAX_DROP) {
    filtered = uFD_do_lpf(filtered, freq);
    lpf = uFD_do_lpf(lpf, filter);
  }

  if(dropCount > uFD_MAX_DROP) return 0;
  if(lpf > uFD_FILTER_HIGHER) return 0;
  if(lpf < uFD_FILTER_LOWER) return 0;

  return freq;
}//uFD_freqMatch