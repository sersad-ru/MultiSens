#include "plgNEO6MV2.h"
#include "uSST.h"
#include <SoftwareSerial.h>


#define RX_PIN P0
#define TX_PIN P1
#define TIMEOUT_MS 1000
#define BUF_SIZE 80

namespace NEO6MV2 {
  // GPS-данные 
  typedef struct {
    uint8_t is_updated;       // Флаг обновления. Ставится функциями разбора и снимается функциями вывода
    char sys_type[3]  = "??"; // Тип системы GP - GPS, GL - Glonass, GA - Galileo, BD - Beidou, GQ - QZSS, GN - Various
    uint8_t sat_total = 0;    // Количество наблюдаемых спутников 
    uint8_t sat_act = 0;      // Количество активных спутников 
    uint8_t hours;            // Часы
    uint8_t minutes;          // Минуты
    uint8_t seconds;          // Секунды
    uint8_t day;              // День
    uint8_t month;            // Месяц
    uint16_t year;            // Год
    char status = '?';        // Достоверность данных 'A'- данные достоверны, 'V' - ошибоные данные     
    uint8_t lat_deg = 0;      // Градусы широты
    uint8_t lat_min = 0;      // Минуты широты
    uint32_t lat_m_frac = 0;  // Доли минут широты
    char lat_dir = '?';       // Направление широты N - север, S - юг
    uint16_t lng_deg = 0;     // Градусы долготы
    uint8_t lng_min = 0;      // Минуты долготы
    uint32_t lng_m_frac = 0;  // Доли минут долготы
    char lng_dir = '?';       // Направление долготы W - запад, E - восток
    uint8_t calc = 0;         // Способ вычисления: 0-недоступно, 1-автономно, 2-дифференциально, 3-PPS, 4-фикс RTK, 5-не фикс RTK, 6-экстраполяция, 7-фикс коорд, 8-симуляця  
  } gpsData;
  

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
    uint8_t st = uSST_strchrn(str, ',', 1); // Ищем первый
    if(!st) return; // Не нашли
    if(str[st + 1] == ',') return; // Параметр пустой
    if(str[st + 1] == 0) return; // Параметр пустой и в самом конце
    // Формат фиксированный, поэтому сразу разбираем    
    gps.hours = _param2long(str + st + 1, 2);
    gps.minutes = _param2long(str + st + 3, 2);
    gps.seconds = _param2long(str + st + 5, 2);
    
    Serial.print("Time: "); 
    Serial.print(gps.hours);
    Serial.print(':');
    Serial.print(gps.minutes);
    Serial.print(':');
    Serial.println(gps.seconds);
  }//_parse_Time


  // Получает широту из заданного параметра (+1 направление)
  void _parse_lattitude(gpsData &gps, char *str, const uint8_t param_num){
    uint8_t st = uSST_strchrn(str, ',', param_num); // Ищем параметр
    if(!st) return; // Не нашли
    if(str[st + 1] == ',') return; // Параметр пустой
    if(str[st + 1] == 0) return; // Параметр пустой и в самом конце
    gps.lat_deg = _param2long(str + st + 1, 2);
    gps.lat_min = _param2long(str + st + 3, 2);
    gps.lat_m_frac = _param2long(str + st + 6, 5, true); // С отрезкой нулей справа

    st = uSST_strchrn(str, ',', param_num + 1); // Ищем параметр + 1
    if(!st) return; // Не нашли
    if(str[st + 1] == ',') return; // Параметр пустой
    gps.lat_dir = str[st + 1];   
  }//_parse_lattitude


  // Получает долготу из заданного параметра (+1 направление)
  void _parse_longtittude(gpsData &gps, char *str, const uint8_t param_num){
    uint8_t st = uSST_strchrn(str, ',', param_num); // Ищем параметр
    if(!st) return; // Не нашли
    if(str[st + 1] == ',') return; // Параметр пустой
    if(str[st + 1] == 0) return; // Параметр пустой и в самом конце
    gps.lng_deg = _param2long(str + st + 1, 3);
    gps.lng_min = _param2long(str + st + 4, 2);
    gps.lng_m_frac = _param2long(str + st + 7, 5, true); // С отрезкой нулей справа

    st = uSST_strchrn(str, ',', param_num + 1); // Ищем параметр + 1
    if(!st) return; // Не нашли
    if(str[st + 1] == ',') return; // Параметр пустой
    if(str[st + 1] == 0) return; // Параметр пустой и в самом конце
    gps.lng_dir = str[st + 1];    
  }//_parse_longtittude

  
  // Разбираем ZDA-строку
  //$GNZDA,102030.000,20,02,2020,00,00*FF
  void _parse_ZDA(gpsData &gps, char *str){
    // Время
    _parse_Time(gps, str);
    
    // День
    uint8_t st = uSST_strchrn(str, ',', 2); // Ищем второй параметр
    if(!st) return; // Не нашли
    if(str[st + 1] == ',') return; // Параметр пустой
    if(str[st + 1] == 0) return; // Параметр пустой и в самом конце

    gps.day = _param2long(str + st + 1, 2);
    gps.month = _param2long(str + st + 4, 2);
    gps.year = _param2long(str + st + 7, 4);
    gps.is_updated = 1;
        
    Serial.print("Date: "); 
    Serial.print(gps.year);
    Serial.print('-');
    Serial.print(gps.month);
    Serial.print('-');
    Serial.println(gps.day);   
  }//_parse_ZDA


  // Разбираем GSV-строку
  // $GLGSV,3,3,09,67,26,161,23,82,70,320,18*FF
  void _parse_GSV(gpsData &gps, char *str){
    uint8_t st = uSST_strchrn(str, ',', 3); // Ищем третий параметр
    if(!st) return; // Не нашли
    if(str[st + 1] == ',') return; // Параметр пустой
    if(str[st + 1] == 0) return; // Параметр пустой и в самом конце
    
    uint8_t en = uSST_strchrn(str, ',', 4); // Ищем конец третьего параметра
    if(!en) en = st + 3; // Не нашли. Используем 2 символа
    gps.sat_total = _param2long(str + st + 1, en - st - 1);
    gps.is_updated = 1;
    
    Serial.print("GSV sat_total: ");
    Serial.println(gps.sat_total);
  }//_parse_GSV


  // Разбираем RMC-строку
  //$GNRMC,102030.000,A,5546.95900,N,03740.69200,E,0.12,49.75,200220,,,A,V*FF
  void _parse_RMC(gpsData &gps, char *str){
    // Время
    _parse_Time(gps, str);

    // Достоверность данных
    uint8_t st = uSST_strchrn(str, ',', 2); // Ищем третий параметр
    if(!st) return; // Не нашли
    if(str[st + 1] == ',') return; // Параметр пустой
    if(str[st + 1] == 0) return; // Параметр пустой и в самом конце
    gps.status = str[st + 1];
    gps.is_updated = 1;

    // Широта
    _parse_lattitude(gps, str, 3);
    
    // Долгота
    _parse_longtittude(gps, str, 5);
    
    // Дата в коротком формате
    st = uSST_strchrn(str, ',', 9); // Ищем 6 параметр
    if(!st) return; // Не нашли
    if(str[st + 1] == ',') return; // Параметр пустой
    if(str[st + 1] == 0) return; // Параметр пустой и в самом конце
    gps.day = _param2long(str + st + 1, 2);
    gps.month = _param2long(str + st + 3, 2);
    gps.year = _param2long(str + st + 5, 2) + 2000;
    gps.is_updated = 1;
    
    Serial.print("RMC status: ");
    Serial.println(gps.status);
    Serial.print("Lattitude: ");
    Serial.print(gps.lat_dir);
    Serial.print(gps.lat_deg);
    Serial.print("°");
    Serial.print(gps.lat_min);
    Serial.print('.');
    Serial.print(gps.lat_m_frac);
    Serial.println('\'');
    Serial.print("Longtittude: ");
    Serial.print(gps.lng_dir);
    Serial.print(gps.lng_deg);
    Serial.print("°");
    Serial.print(gps.lng_min);
    Serial.print('.');
    Serial.print(gps.lng_m_frac);
    Serial.println('\'');

    Serial.print("Date: "); 
    Serial.print(gps.year);
    Serial.print('-');
    Serial.print(gps.month);
    Serial.print('-');
    Serial.println(gps.day);      
  }//_parse_RMC


  // Разбираем GGA-строку
  //$GNGGA,102030.000,5546.95900,N,03740.69200,E,1,08,2.7,142.0,M,0.0,M,,*FF
  void _parse_GGA(gpsData &gps, char *str){
    // Время
    _parse_Time(gps, str);
    // Широта
    _parse_lattitude(gps, str, 2);  
    // Долгота
    _parse_longtittude(gps, str, 4);

    // Способ вычисления координат
    uint8_t st = uSST_strchrn(str, ',', 6); // Ищем параметр
    if(!st) return; // Не нашли
    if(str[st + 1] == ',') return; // Параметр пустой
    if(str[st + 1] == 0) return; // Параметр пустой и в самом конце
    gps.calc = _param2long(str + st + 1, 1);
        
    // Количество активных спутников   
    st = uSST_strchrn(str, ',', 7); // Ищем параметр
    if(!st) return; // Не нашли
    if(str[st + 1] == ',') return; // Параметр пустой
    if(str[st + 1] == 0) return; // Параметр пустой и в самом конце
    gps.sat_act = _param2long(str + st + 1, 2);
    gps.is_updated = 1;    

    Serial.print("GGS Sattelite active: ");
    Serial.println(gps.sat_act);

    Serial.print("Calc type: ");
    Serial.println(gps.calc);
    
    Serial.print("Lattitude: ");
    Serial.print(gps.lat_dir);
    Serial.print(gps.lat_deg);
    Serial.print("°");
    Serial.print(gps.lat_min);
    Serial.print('.');
    Serial.print(gps.lat_m_frac);
    Serial.println('\'');
    Serial.print("Longtittude: ");
    Serial.print(gps.lng_dir);
    Serial.print(gps.lng_deg);
    Serial.print("°");
    Serial.print(gps.lng_min);
    Serial.print('.');
    Serial.print(gps.lng_m_frac);
    Serial.println('\'');
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
}// namespace

using namespace NEO6MV2;

/*
 * if((res = getRes()) > 0) ???
 * Поиск параметра убрать в функцию, отдающую позицию или -1, если не найден. Поиск символа переделать на int8_t с возвратом -1 если не найден.
 * Широту и долготу разбирать одной функцией. Параметр - длина поля градусов (2 или 3). Результат писать в поле типа grad (см. ниже)
 * В записи данный GPS сделать поля lat и lng типа grad(запись на градусы, минуты, доли минут и направление)
 * Сделать нормальный вывод на экран (с выбором режима: Lat, Long, Sat, Date) и в сериал.
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

  uint32_t ser_speed = uSST_FindSpeed(ser, &_probe, buf, BUF_SIZE, TIMEOUT_MS);
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

  // Main loop
  while(1){
    if(uSST_ReadString(ser, buf, BUF_SIZE, TIMEOUT_MS)){ 
      _parse(gps, buf, BUF_SIZE);
      Serial.println(buf);
     }//if
    //if (ser.available()) Serial.write(ser.read());
  }//while
}//plgNEO6MV2
