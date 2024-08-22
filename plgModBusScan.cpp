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

#include "plgModBusScan.h"
#include "uSST.h"
#include <SoftwareSerial.h>

#define TX_PIN P0
#define RX_PIN P1
#define CTRL_PIN P2

#define SCAN_MAX 5     // Scan up to first SCAN_MAX devices
#define TIMEOUT_MS 150 // Device reply timeout ms 
#define FIRST_ADDR 1   // First address to find
#define LAST_ADDR  247 // Last address to find

#define BUF_SIZE 16

namespace ModBusScan {
   
  // Первый байт в 16-битном слове будет старшим
  #define htons(x) ((((uint16_t)(x) & 0xFF00) >> 8) | ((uint16_t)(x) & 0xFF) << 8)

  typedef struct {
    uint8_t addr; // Адрес
    uint8_t cmd;  // Команда
    uint16_t reg; // Стартовый адрес регистров
    uint16_t cnt; // Количество регистров, которые надо прочесть 
    uint16_t crc; // CRC
  } testPkt; 


  // Посчитать CRC для len байт буфера
  uint16_t _calcCRC(uint8_t* buf, const uint8_t buf_size, const uint8_t len){
    if(len >= buf_size) return 0; // Вылезаем за границы буфера
    uint16_t crc = 0xFFFF;
    for(uint8_t i = 0; i < len; i++){
      crc ^= buf[i];
      for(uint8_t j = 0; j < 8; j++){
        if(crc & 0x0001){
          crc >>= 1;
          crc ^= 0xA001;
        }//if
        else crc >>= 1;
      }//for
    }//for
    return crc; // Сам нормально отдается младшим байтом вперед как надо
  }//_calcCRC


  // Отправить тестовый пакет
  void _sendPkt(Stream &ser, uint8_t* buf, const uint8_t buf_size, const uint8_t addr, const uint8_t cmd){
    if(sizeof(testPkt) > buf_size) return; // Слишком маленький буфер
    // Формируем тестовый пакет  
    testPkt *pkt = (testPkt*)buf;
    pkt->addr = addr;  // Адрес
    pkt->cmd = cmd;   // Читаем AI регистр
    pkt->reg = htons(0x0001); // Номер регистра 
    pkt->cnt = htons(0x0001); // Сколько регистров читаем
    pkt->crc = _calcCRC(buf, buf_size, sizeof(testPkt) - 2); // Само поле CRC не обсчитывать, поэтому -2

    while(ser.read() >= 0); // Чистим буфер ком-порта  
    // Отправляем
    digitalWrite(CTRL_PIN, HIGH);
    ser.write(buf, sizeof(testPkt));
    // Вот тут нужна ли задержка, чтобы буфер успел отправиться?
    digitalWrite(CTRL_PIN, LOW);    
  }//_sendPkt


 // Проверить, пришел ли корректный ответ (сошлась ли CRC, не было ли таймаута и не вышли ли за границы буфера)
 // 1 - да, 0 - нет.
 uint8_t _checkReply(Stream &ser, uint8_t* buf, const uint8_t buf_size, const uint32_t timeout_ms){
   uint8_t i = 0;
   uint32_t t_start = millis(); // Время начала работы
   while((millis() - t_start) < timeout_ms){
    while(ser.available()){
      buf[i++] = ser.read();
      if(i > buf_size) return 0; // Переполнение буфера
    }//while
   }//while 
   if(i < 5) return 0; // Таймаут. Слишком мало данных в буфере (для ошибки надо хотя бы 5)
   uint16_t* crc = (uint16_t*) &buf[i - 2]; // Совмещаем указатель на crc с позицией в буфере (два последних байта)     
   return *crc == _calcCRC(buf, buf_size, i - 2); // Само поле CRC не обсчитывать, поэтому -2). Возвращаем, сошлось ли CRC. Что именно там было в ответе нам все равно.
 }// _checkReply


 // Пытается соединится с устройством с заданным адресом  на заданной конфигурации порта, используя разные команды. КОМ-порт должен быть уже открыт!
 // 1 - устройство ответило, 0 - не ответило
 uint8_t _checkDevice(Stream &ser, const uint8_t addr, uint8_t *commands, const uint8_t commands_size, uint8_t* buf, const uint8_t buf_size, const uint32_t timeout_ms){
   for(uint8_t i = 0; i < commands_size; i++){
     _sendPkt(ser, buf, buf_size, addr, commands[i]); // Посылаем запрос с очередной командой
     if(_checkReply(ser, buf, buf_size, timeout_ms)) return 1; // Если устройство ответило выходим.
   }//for
   return 0; // Устройство не отозвалось ни на одну команду
 }//_checkDevice


 // Сформировать в буфере строку, содержащую текстовое представление конфигурации порта
 const char* _conf2str(const uint8_t conf){
  switch(conf){
    case(SERIAL_8N1): return "8N1";
    case(SERIAL_8E1): return "8E1";
    case(SERIAL_8O1): return "8O1";
    case(SERIAL_8N2): return "8N2";
    case(SERIAL_8E2): return "8E2";
    case(SERIAL_8O2): return "8O2";    
    default: return nullptr;
  }//switch  
 }//_conf2str


 // Высести секунды в формате ЧЧ:MM:CC
 void _printSecondsAsTime(Print &p, const uint32_t sec){
   uint32_t val = sec / 3600; // часы  
   if(val){
     p.print(val);
     p.print(':');
   }//if   
   val = (sec % 3600) / 60; // минуты
   if((val > 0) && (val < 10)) p.print('0');
   p.print(val);
   p.print(':');
   val = (sec % 3600) % 60; // секунды
   if(val < 10) p.print('0');
   p.print(val);   
 }//_printSecondsAsTime
 

 // Поиск устройств и помещение их в массив. Возвращает количество найденных устройств (0 - если ничего не нашли).
 // Описание устройства
 typedef struct {
  uint8_t addr;   // Адрес устройства
  uint32_t speed; // Скорость порта
  uint8_t config; // Конфигурация порта
 } mbDevice; 

 // Прототип callback-а на получение первого устройства
 typedef void (*firstFound)(void);
 
 // Прототип callback-а на прогресс. Получает текущие выполненные проценты, умноженные на 10, оставшееся время в секундах и сколько уже найдено
 typedef void (*progressStep)(const uint16_t percent, const uint32_t left_sec, const uint8_t found);

 // Прототип callback-а на прерывание. Пока возвращает 0 все работает. Как только вернет 1 - процесс будет прерван
 typedef uint8_t (*stopIt)(void);

// Прототип callback-а на итерацию. Вызывается внутри цикла перебора адресов (не захватывает перебор команд)
 typedef void (*searchStep)(void);

 void _openPort(SoftwareSerial &ser, const uint32_t speed, const uint8_t config){
  ser.begin(speed);
 }//openPort
 void _openPort(HardwareSerial &ser, const uint32_t speed, const uint8_t config){
  ser.begin(speed, config);
 }//openPort


 template <typename T>
 uint8_t _findSpeed(const uint8_t config, const uint8_t config_size, 
                    const uint8_t addr_from, const uint8_t addr_to,
                    uint32_t *speeds, const uint8_t speeds_size,
                    uint8_t *commands, const uint8_t commands_size,
                    mbDevice *devices, const uint8_t max_devices, 
                    T &ser, uint8_t* buf, const uint8_t buf_size, 
                    const uint32_t timeout_ms, const uint32_t t_start_ms, const int16_t time_corr_pm,
                    firstFound ff_callback, progressStep prog_callback, stopIt stop_callback, searchStep step_callback){
                      
  static uint8_t dev_num = 0; // Индекс текущего устройства
  static uint32_t old_elapsed = 0; 
  
  const uint16_t total = (config_size * speeds_size) * (max(addr_from, addr_to) - min(addr_from, addr_to) + 1); // Сколько всего итераций, не считая цикла по командам
  const uint32_t time_total_sec = (total * commands_size * timeout_ms) / 1000 + (((total * commands_size * timeout_ms) / 1000) * time_corr_pm) / 1000; // Сколько всего нужно времени
  static uint16_t cnt = 0; // Счетчик итераций (всего, кроме перебора команд)
    
  if(!max_devices) return 0; // Если максимум = 0, то вообще искать нечего
  
  //((SoftwareSerial *)&ser)->begin(9600); //!!! работает, но стремно!
  for(uint8_t spd = 0; spd < speeds_size; spd++){ // Перебираем скорости
    //Настраиваем порт
    _openPort(ser, speeds[spd], config);
    for(uint8_t addr = addr_from;; (addr_to >= addr_from) ? addr++ : addr--){ // Перебираем адреса. Цикл выходит в конце отдельным условием, чтобы можно было ходить в любую сторону и включать границы диапазонов
      // Проверяем, не прервали ли процесс
      if(stop_callback && stop_callback()){
        ser.end(); // Закрываем порт
        return dev_num;
      }//if

      // Вызываем коллбэк шага итерации
      if(step_callback) step_callback();
      
      // Считаем прогресс в диапазоне 0..1000 (проценты * 10). Выводим раз в секунду.
      if(prog_callback){       
        uint32_t elapsed_sec = ((millis() - t_start_ms) / 1000); // Секунды, прошедшие с начала работы
        if(old_elapsed != elapsed_sec){ // Пора выводить прогресс (секунда прошла)
          old_elapsed = elapsed_sec;
          // Хитрим, что б время не ушло в минус из-за неточности коллбэков
          prog_callback(map(cnt, 0, total, 0, 1000), (time_total_sec > elapsed_sec) ? (time_total_sec - elapsed_sec) : 0, dev_num); 
        }//if old_elapsed
      }//if prog_callback  
    
      cnt++; // Счетчик итераций
      
      // Ищем устройство 
      if(_checkDevice(ser, addr, commands, commands_size, buf, buf_size, timeout_ms)){ // Ищем устройство 
        // Устройство есть! :)
        devices[dev_num++] = {addr, speeds[spd], config}; // Сохранили устройство в массив
        if((dev_num == 1) && (ff_callback)) ff_callback(); // Вызвали колбэк на первое устройство
        if(dev_num == max_devices){ // Хватит уже. Нашли максимум
          ser.end(); // Закрываем порт
          return dev_num;
        }//if
      }//if
      
      if(addr == addr_to) break; // Тут выход из цикла, чтобы края диапазонов включались и направление обхода было произвольное
    }//for addr
    ser.end(); // Закрываем порт
  }//for spd
  return dev_num;
 }//_findSpeed

 
 // Найти устройства и заполнить их параметрами массив устройств. Вернуть количество найденных устройств.
 // * device - массив устройств mbDevice
 // * max_devices - искать не более этого количества устройств
 // * ser - ком-порт
 // * addr_from - с какого адреса начинать искать
 // * addr_to - до какого адреса (включительно) искать
 // * speeds - массив скоростей порта
 // * speeds_size - размер массива скоростей
 // * configs - массив настроек порта
 // * configs_size - размер массива настроек
 // * commands - массив команд  
 // * commands_size - размер массива команд
 // * buf - рабочий буфер
 // * buf_size - размер рабочего буфера
 // * timeout_ms - сколько ждать ответа от устройства (в мс)
 // * time_corr_pm - коррекция рассчета общего времени работы в промилях. Когда написаны все коллбэки замеряем реальное время и рассчетное. 
 //                  К рассчетному будет добавлено оно же еще раз, но умноженное на этот коэффициент и разделеное на 1000. т.е. 0,1%
 // * ff_callback - функция будет вызвана при нахождении первого устройства (формат firstFound)
 // * prog_callback - функция будет вызваться по ходу поиска (формат progressStep)
 // * stop_callback - пока вункция возвращает 0, процесс идет. Как только вернет 1 - поиск будет прерван
 // * step_callback - вызывается внутри цикла перебора адресов. Не затрагивает перебор команд
 uint8_t _findDevices(mbDevice * devices, const uint8_t max_devices, HardwareSerial &ser, 
                      const uint8_t addr_from, const uint8_t addr_to,
                      uint32_t *speeds, const uint8_t speeds_size, // Массив скоростей
                      uint8_t *configs, const uint8_t configs_size, // Массив конфигураций
                      uint8_t *commands, const uint8_t commands_size, // Массив команд
                      uint8_t* buf, const uint8_t buf_size, const uint32_t timeout_ms, const int16_t time_corr_pm = 0,
                      firstFound ff_callback = nullptr, progressStep prog_callback = nullptr, stopIt stop_callback = nullptr, searchStep step_callback = nullptr){                 
  uint8_t res = 0;
  uint32_t t_start = millis();
  for(uint8_t i = 0; i < configs_size; i++){ // Перебираем конфигурации
    res = _findSpeed(configs[i], configs_size, // Настройки порта (настройка, номер настройки, всего настроек)
                     addr_from, addr_to, // Диапазон адресов (от и до)
                     speeds, speeds_size, // Массив скоростей порта
                     commands, commands_size, // Массив команд
                     devices, max_devices, ser, buf, buf_size, 
                     timeout_ms, t_start, time_corr_pm,
                     ff_callback, prog_callback, stop_callback, step_callback);
    if(res >= max_devices) break; // Уже нашли достаточно устройств
  }//for conf  
  return res;
 }//_findDevices

 uint8_t _findDevices(mbDevice * devices, const uint8_t max_devices, SoftwareSerial &ser, 
                      const uint8_t addr_from, const uint8_t addr_to,
                      uint32_t *speeds, const uint8_t speeds_size, // Массив скоростей
                      uint8_t *configs, const uint8_t configs_size, // Массив конфигураций
                      uint8_t *commands, const uint8_t commands_size, // Массив команд
                      uint8_t* buf, const uint8_t buf_size, const uint32_t timeout_ms, const int16_t time_corr_pm = 0,
                      firstFound ff_callback = nullptr, progressStep prog_callback = nullptr, stopIt stop_callback = nullptr, searchStep step_callback = nullptr){
                        
  return _findSpeed(SERIAL_8N1, 1, // Настройки порта (настройка, номер настройки, всего настроек)
                    addr_from, addr_to, // Диапазон адресов (от и до)
                    speeds, speeds_size, // Массив скоростей порта
                    commands, commands_size, // Массив команд
                    devices, max_devices, ser, buf, buf_size, 
                    timeout_ms, millis(), time_corr_pm,
                    ff_callback, prog_callback, stop_callback, step_callback); // Для SoftSerial только одна конфигурация                       
 }//_findDevices


 // Реакция на первое найденное устройство
 void _first_found(){
   Serial.println("\n---===First found! :)===---\n");
 }//_first_found


 // Вывод прогресса на экран и в порт
 void _print_progress(Print &p, const uint16_t percent, const uint32_t left_sec, const uint8_t found){
   p.print('[');
   p.print(found);
   p.print("] ");
   core.printValScale(p, percent);
   p.print("% ");
   _printSecondsAsTime(p, left_sec);
   p.println(); 
 }//_print_progress

 
 // Отображение прогресса (Проценты, умноженные на 10)
 void _show_progress(const uint16_t percent, const uint32_t left_sec, const uint8_t found){
   core.moveCursor(0, 1);  
   _print_progress(core, percent, left_sec, found);
   _print_progress(Serial, percent, left_sec, found);
 }//_first_found
 

 // Прерывание процесса
 // Прототип callback-а на прерывание. Пока возвращает 0 все работает. Как только вернет 1 - процесс будет прерван
 uint8_t _abort(){
  return core.getButton() == SELECT;
 }//_abort


 // Вывод информации об устройстве
 void _print_device(Print &p, mbDevice * devices, const uint8_t device_num, const uint8_t total){
  p.print(device_num + 1);
  p.print('/');
  p.print(total);
  p.print(" ");
  p.print(devices[device_num].addr);
  p.print(' ');
  p.print(devices[device_num].speed);
  p.print(' ');
  p.println(_conf2str(devices[device_num].config));
 }//_print_device
}//namespace


using namespace ModBusScan;
// == Main plugin function ==
void plgModBusScan(){
  const uint32_t speeds[] = {9600, 19200, 115200, 57600, 38400, 4800, 2400}; // Варианты скорости порта
  const uint8_t configs[] = {SERIAL_8N1, SERIAL_8E1, SERIAL_8O1, SERIAL_8N2, SERIAL_8E2, SERIAL_8O2}; // Варианты конфигурации порта                        
  const uint8_t commands[] = {0x11, 0x08, 0x04, 0x03, 0x02, 0x01}; // Список команд в порядке вероятности ответа
      
  uint8_t buf[BUF_SIZE]; // Буфер для работы
  mbDevice devices[SCAN_MAX]; // Массив найденных устройств
  
  // Init
  SoftwareSerial ser(RX_PIN, TX_PIN);
  
  pinMode(CTRL_PIN, OUTPUT);
  digitalWrite(CTRL_PIN, LOW);
  
  uint32_t t_start = millis();
/*
 * Параметр  time_corr_pm считается так:
 * 1. Полностью пишем все callback-и
 * 2. Запускаем при time_corr_pm = 0
 * 3. Смотрим рассчетное время. Например 1:16, т.е. 73 секунды
 * 4. Замеряем реальное время. Например 77 секунд 
 * 5. Т.е. надо добавить 4 секунды
 * 5. 77 / 100 = 0.77 =>  4 / 0.77 = 5.194..., т.е надо добавить 5,1% 
 * 7. time_corr_pm = 51
*/
  uint8_t found = _findDevices(devices, SCAN_MAX, ser, 
                               FIRST_ADDR, LAST_ADDR, // Диапазон адресов для поиска
                               speeds,  arraySize(speeds), // Массив скоростей порта и его размер
                               configs, arraySize(configs), // Массив настроек порта и его размер
                               commands, arraySize(commands), // Массив команд и его размер
                               buf, BUF_SIZE, TIMEOUT_MS, 31, &_first_found, &_show_progress, &_abort);
  uint32_t elapsed = (millis() - t_start) / 1000;                              
  Serial.print(found);
  Serial.print(F(" device(s) found in "));
  Serial.print(elapsed);
  Serial.println(F(" seconds"));

  if(!found){
    Serial.println(FF(uSST_NO_DEV_MSG));
    core.moveCursor(0, 1);
    core.println(FF(uSST_NO_DEV_MSG));
    while(1); // Ничего не нашли. Выводим сообщение и виснем
  }//if

  Serial.println(F("[N] Address Port"));
  for(uint8_t i = 0; i < found; i++)
    _print_device(Serial, devices, i, found); 
    
  // Main loop
  int8_t current = 0;
  while(1){
    _print_device(core, devices, current, found); 
    // Process user input    
    switch (core.wait4Button()) {
      case UP: current--; break;
      case UP_LONG: current = 0; break;
      
      case DOWN: current++; break;
      case DOWN_LONG: current = found - 1; break;
      
      default: break;
    }//switch    
    current = constrain(current, 0, found - 1);
  }//while  
}//plgModBusScan
