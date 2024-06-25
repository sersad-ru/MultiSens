#include "plgNEO6MV2.h"
#include "uSST.h"
#include <SoftwareSerial.h>


#define RX_PIN P0
#define TX_PIN P1

#define TIMEOUT_MS 1000
#define BUF_SIZE 80
#define DISPLAY_DELAY_MS 1000

namespace NEO6MV2 {
  
// Режим отображения 
#define MODE_LAT 0  // Отображаем широту
#define MODE_LNG 1  // Отображаем долготу
#define MODE_SAT 2  // Отображаем спутники
#define MODE_DAT 3  // Отображаем дату

  
  // Градусы
  typedef struct {
    uint16_t deg = 0;    // Градусы
    uint8_t  min = 0;      // Минуты
    uint32_t m_frac = 0; // Доли минут
    char     dir = '?';  // Направление (N, S, W, E)
  } gpsDegree;
  
  // GPS-данные The
  typedef struct {
    char sys_type[3]  = "??"; // Тип системы GP - GPS, GL - Glonass, GA - Galileo, BD - Beidou, GQ - QZSS, GN - Various
    uint8_t sat_total = 0;    // Количество наблюдаемых спутников 
    uint8_t sat_act = 0;      // Количество активных спутников 
    uint8_t hours = 0;        // Часы
    uint8_t minutes = 0;      // Минуты
    uint8_t seconds = 0;      // Секунды
    uint8_t day = 0;          // День
    uint8_t month = 0;        // Месяц
    uint16_t year = 0;        // Год
    char status = '?';        // Достоверность данных 'A'- данные достоверны, 'V' - ошибоные данные     
    gpsDegree lat;            // Широта
    gpsDegree lng;            // Долгота      
    uint8_t calc = 0;         // Способ вычисления: 0-недоступно, 1-автономно, 2-дифференциально, 3-PPS, 4-фикс RTK, 5-не фикс RTK, 6-экстраполяция, 7-фикс коорд, 8-симуляця  
  } gpsData;

/*
0 - fix not available,
1 - GPS fix,
2 - Differential GPS fix
*/

  // Функия проверки соединения по COM-порту
  uint8_t _probe(const SoftwareSerial &ser, char* buf, const uint8_t buf_size, const uint32_t timeout_ms){
    const char* prefix[] = {"GP", "GL", "GA", "BD", "GQ", "GN"};
    for(uint8_t i = 0; i < 3; i++){ // Пробуем 3 раз
      if(!uSST_ReadString(ser, buf, buf_size, timeout_ms)) continue; // Не прочлась строка
      if(buf[0] != '$') continue; // Первый символ не '$'
      buf++; // Смотрим начиная со второго символа
      for(uint8_t j = 0; j < arraySize(prefix); j++)
        if(!strncmp(buf, prefix[j], j)) return 1; // Нашли
    }//for
    return 0;
  }//_probe


  // Ищем позицию заданного по счету параметр. -1, если не нашли
  int8_t _locate_param(const char *str, const uint8_t n){
    int8_t res = uSST_strchrn(str, ',', n); 
    if(res < 0) return -1; // Не нашли
    if(str[res + 1] == ',') return -1; // Параметр пустой
    if(str[res + 1] == 0) return -1; // Параметр пустой и в самом конце
    return res;
  }//_locate_param


  // Конвертируем параметр в int по его размеру.
  // Если is_frac = 1, то отрезаем нули справа
  uint32_t _param2long(char *str, uint8_t param_size, const uint8_t is_frac = 0){
    while((is_frac) && (param_size > 0) && (str[param_size - 1] == '0')) param_size--; // Отрезаем нули справа  
    char old = str[param_size];
    str[param_size] = 0;
    int32_t res = atol(str);
    str[param_size] = old;
    return res;
  }//_param2int


  // Получаем время
  void _parse_Time(gpsData &gps, char *str){
    int8_t st = _locate_param(str, 1); // Ищем первый
    if(st < 0) return; // Не нашли
    // Формат фиксированный, поэтому сразу разбираем    
    gps.hours = _param2long(str + st + 1, 2);
    gps.minutes = _param2long(str + st + 3, 2);
    gps.seconds = _param2long(str + st + 5, 2);
  }//_parse_Time


  // Получить градусы
  void _parse_degree(gpsDegree &degree, const char *str, const uint8_t param_num, const uint8_t deg_size){
    int8_t st = _locate_param(str, param_num); // Ищем параметр
    if(st < 0) return; // Не нашли
    degree.deg = _param2long(str + st + 1, deg_size);
    degree.min = _param2long(str + st + deg_size + 1, 2);
    degree.m_frac = _param2long(str + st + deg_size + 4, 5, true); // С отрезкой нулей справа

    st = _locate_param(str, param_num + 1);// Ищем параметр + 1
    if(st < 0) return; // Не нашли
    degree.dir = str[st + 1];       
  }//_parse_degree

 
  // Разбираем ZDA-строку
  //$GNZDA,102030.000,20,02,2020,00,00*FF
  void _parse_ZDA(gpsData &gps, char *str){
    // Время
    _parse_Time(gps, str);
    
    // День
    int8_t st = _locate_param(str, 2); // Ищем второй параметр
    if(st < 0) return; // Не нашли

    gps.day = _param2long(str + st + 1, 2);
    gps.month = _param2long(str + st + 4, 2);
    gps.year = _param2long(str + st + 7, 4);
  }//_parse_ZDA


  // Разбираем GSV-строку
  // $GLGSV,3,3,09,67,26,161,23,82,70,320,18*FF
  void _parse_GSV(gpsData &gps, char *str){
    int8_t st = _locate_param(str, 3); // Ищем третий параметр
    if(st < 0) return; // Не нашли
    
    int8_t en = _locate_param(str, 4); // Ищем конец третьего параметра
    if(en < 0) en = st + 3; // Не нашли. Используем 2 символа
    gps.sat_total = _param2long(str + st + 1, en - st - 1);
  }//_parse_GSV


  // Разбираем RMC-строку
  //$GNRMC,102030.000,A,5546.95900,N,03740.69200,E,0.12,49.75,200220,,,A,V*FF
  void _parse_RMC(gpsData &gps, char *str){
    // Время
    _parse_Time(gps, str);

    // Достоверность данных
    int8_t st = _locate_param(str, 2); // Ищем третий параметр
    if(st < 0) return; // Не нашли
    gps.status = str[st + 1];

    // Широта
    _parse_degree(gps.lat, str, 3, 2);
    
    // Долгота
    _parse_degree(gps.lng, str, 5, 3);
    
    // Дата в коротком формате
    st = _locate_param(str, 9); // Ищем 6 параметр
    if(st < 0) return; // Не нашли
    gps.day = _param2long(str + st + 1, 2);
    gps.month = _param2long(str + st + 3, 2);
    gps.year = _param2long(str + st + 5, 2) + 2000;
  }//_parse_RMC


  // Разбираем GGA-строку
  //$GNGGA,102030.000,5546.95900,N,03740.69200,E,1,08,2.7,142.0,M,0.0,M,,*FF
  void _parse_GGA(gpsData &gps, char *str){
    // Время
    _parse_Time(gps, str);
    // Широта
    _parse_degree(gps.lat, str, 2, 2);
    // Долгота
    _parse_degree(gps.lng, str, 4, 3);

    // Способ вычисления координат
    int8_t st = _locate_param(str, 6); // Ищем параметр
    if(st < 0) return; // Не нашли
    gps.calc = _param2long(str + st + 1, 1);
        
    // Количество активных спутников   
    st = _locate_param(str, 7); // Ищем параметр
    if(st < 0) return; // Не нашли
    gps.sat_act = _param2long(str + st + 1, 2);
  }//_parse_GGA

  
  // Разбирает строку и складывает даные в глобальные переменные
  void _parse(gpsData &gps, char *buf, const uint8_t buf_size){
    if(buf[0] != '$') return; // Нет ведущего символа

    // Тип системы
    strncpy(gps.sys_type, buf + 1, 2);

    // Типы сообщений
    if(!strncmp(buf + 3, "ZDA", 3)){
       _parse_ZDA(gps, buf);
       return;
    }//if

    if(!strncmp(buf + 3, "GSV", 3)){
       _parse_GSV(gps, buf);
       return;
    }//if

    if(!strncmp(buf + 3, "RMC", 3)){
       _parse_RMC(gps, buf);
       return;
    }//if

    if(!strncmp(buf + 3, "GGA", 3)){
       _parse_GGA(gps, buf);
       return;
    }//if    
  }//_parse


//GP - GPS, GL - Glonass, GA - Galileo, BD - Beidou, GQ - QZSS, GN - Various        
  void _print_type(Print &p, const char* type){
    if(!strncmp(type, "GP", 2)){
       p.print(F("GPS"));
       return;
    }//if

    if(!strncmp(type, "GL", 2)){
       p.print(F("Glonass"));
       return;
    }//if

    if(!strncmp(type, "GA", 2)){
       p.print(F("Galileo"));
       return;
    }//if

    if(!strncmp(type, "BD", 2)){
       p.print(F("Beidou"));
       return;
    }//if
    
    if(!strncmp(type, "GQ", 2)){
       p.print(F("QZSS"));
       return;
    }//if
    
    if(!strncmp(type, "GN", 2)){
       p.print(F("Various"));
       return;
    }//if

    p.print(F("??"));
  }//_print_type


  void _display(gpsData &gps, const int8_t mode) {
    static uint32_t last_time = 0;
    if(millis() - last_time < DISPLAY_DELAY_MS) return;
    last_time = millis();
    
    core.moveCursor(0, 1);
    switch(mode){
      case MODE_LAT: 
        core.print(gps.status);
        core.print(gps.calc);
        core.print(' ');
        core.print(gps.lat.dir);
        core.print(gps.lat.deg);
        core.print(MS_SYM_DEGREE_CODE);
        core.print(gps.lat.min);
        core.print('.');
        core.print(gps.lat.m_frac);
        core.println('\'');        
      break;

      case MODE_LNG: 
        core.print(gps.status);
        core.print(gps.calc);
        core.print(' ');
        core.print(gps.lng.dir);
        core.print(gps.lng.deg);
        core.print(MS_SYM_DEGREE_CODE);
        core.print(gps.lng.min);
        core.print('.');
        core.print(gps.lng.m_frac);
        core.println('\'');        
      break;

      case MODE_SAT: 
        _print_type(core, gps.sys_type);       
        core.print(' ');
        core.print(gps.sat_act);
        core.print('/');
        core.print(gps.sat_total);
        core.print(' ');
        core.print(gps.status);
        core.print(' ');
        core.println(gps.calc);
      break;
      
      case MODE_DAT: 
        core.print(gps.year);
        core.print('-');
        core.print(core.rAlign(gps.month, 2, '0'));
        core.print('-');
        core.print(core.rAlign(gps.day, 2, '0'));
        core.print(' ');
        core.print(core.rAlign(gps.hours, 2, '0'));
        core.print(':');
        core.print(core.rAlign(gps.minutes, 2, '0'));
        core.print(':');
        core.println(core.rAlign(gps.seconds, 2, '0'));
      break;
      
      default: break;
    }//switch

    Serial.print(gps.year);
    Serial.print('-');
    Serial.print(core.rAlign(gps.month, 2, '0'));
    Serial.print('-');
    Serial.print(core.rAlign(gps.day, 2, '0'));
    Serial.print(' ');
    Serial.print(core.rAlign(gps.hours, 2, '0'));
    Serial.print(':');
    Serial.print(core.rAlign(gps.minutes, 2, '0'));
    Serial.print(':');
    Serial.print(core.rAlign(gps.seconds, 2, '0'));
    Serial.print(' ');
    _print_type(Serial, gps.sys_type);       
    Serial.print(' ');
    Serial.print(gps.status);
    Serial.print(' ');
    Serial.print(gps.calc);    
    Serial.print(" (");
    Serial.print(gps.sat_act);
    Serial.print('/');
    Serial.print(gps.sat_total);    
    Serial.print(") ");
    Serial.print(gps.lat.dir);
    Serial.print(gps.lat.deg);
    Serial.print("°");
    Serial.print(gps.lat.min);
    Serial.print('.');
    Serial.print(gps.lat.m_frac);
    Serial.print("' ");        
    Serial.print(gps.lng.dir);
    Serial.print(gps.lng.deg);
    Serial.print("°");
    Serial.print(gps.lng.min);
    Serial.print('.');
    Serial.print(gps.lng.m_frac);    
    Serial.println('\'');  
  }//_display
}// namespace


using namespace NEO6MV2;

/*
 * Написать доку
 * Проверить при ловящемся GPS
*/
// == Main plugin function ==
void plgNEO6MV2(){
  char buf[BUF_SIZE]; // Буфер для чтения строки
  gpsData gps;

  // Init
  SoftwareSerial ser(RX_PIN, TX_PIN);
  
  // Dispaly init
  core.moveCursor(0, 1);
  core.println(FF(uSST_CONNECTING_MSG));
  Serial.println(FF(uSST_CONNECTING_MSG));
  
  uint32_t ser_speed = uSST_FindSpeed(ser, &_probe, buf, BUF_SIZE, TIMEOUT_MS, 9600);
  if(!ser_speed){ // Не нашли устройство
    core.moveCursor(0, 1);
    core.println(FF(uSST_NO_DEV_MSG));
    Serial.println(FF(uSST_NO_DEV_MSG));
    while(1);
  }//if

  core.moveCursor(0, 1);
  core.print(FF(uSST_FOUND_MSG));
  core.println(ser_speed);
  
  Serial.print(FF(uSST_FOUND_MSG));
  Serial.print(ser_speed);
  Serial.println(F("bps"));
  ser.begin(ser_speed);

  int8_t mode = MODE_LAT;
  uint8_t raw2serial = 0;
  
  // Main loop  
  while(1){
    // Получаем и разбираем данные от приемника
    if(uSST_ReadString(ser, buf, BUF_SIZE, TIMEOUT_MS)){ 
      _parse(gps, buf, BUF_SIZE);
      if(raw2serial) Serial.println(buf);
     }//if
     
    // Process user input    
    switch (core.getButton()) {
      case UP:
      case UP_LONG: mode --; break;
      
      case DOWN:
      case DOWN_LONG: mode ++; break;

      case SELECT: raw2serial = !raw2serial; break;  
      case SELECT_LONG: break;   
      
      default: break;
    }//switch

    if(mode < 0) mode = MODE_DAT;
    if(mode > MODE_DAT) mode = MODE_LAT;
    _display(gps, mode);
  }//while
}//plgNEO6MV2
