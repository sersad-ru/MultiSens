#include "plgW5500Lite.h"
#include <SPI.h>
#include <Ethernet.h>

#define CS_PIN   P0
#define MOSI_PIN P2
#define MISO_PIN P3
#define SCK_PIN  P4


#define SPI_CONFIG SPISettings(14000000, MSBFIRST, SPI_MODE0)

/*
  В устройстве хранение big-endian. Т.е. IP 172.16.0.1 хранится как AC 10 00 01
  В микроконтроллере little-endian, т.е. 172.16.0.1 в uint32_t должно быть 0x01 00 10 AC
  Тоже самое и для 16-битных значений, которые передаются побайтово, например, номера портов (htons в помощь).
  Исключение - адреса, которые передаются отдельной командой и их конвертацией занимается SPI.
*/ 

// DHCP - около 4Kb

// ---- Hardware ----
#define MR_REG          0x0000 // Регистр режима (8)
#define GW_REG          0x0001 // Регистр адреса шлюза (4)
#define MSK_REG         0x0005 // Регистр маски подсети (4)
#define SRC_IP_REG      0x000F // Регистр IP-адреса отправителя (4)
#define SRC_MAC_REG     0x0009 // Регистр MAC-адреса отправлителя (6)
#define RTR_REG         0x0019 // Регистр времени таймаута (1 = 100 us)(2)
#define RTC_REG         0x001B // Регистр количества повторов (8)

#define PHYCFG_REG      0x002E // Регистр конфигурации среды бит 0: 1 подключено, 0 выключено, бит 1: 1 - 100м/б, 0 - 10м/б, бит 3: 1-дуплекс, 0 - полудуплекс
#define VERSION_REG     0x0039 // Регистр версии чипа (для W5500 должен быть = 4)

#define VERSION_VAL     0x04   // Версия чипа (для W5500 должна быть = 4)

// ---- Socket ----
#define SOCK_NUM      0x08   // Количество сокетов
#define SOCK_SIZE     2048   // Размер буфера сокета 
#define SOCK_REG_SIZE 0x0100 // Размер блока регистров сокета
#define SOCK_BASE     0x1000 // Базовое смещение для блоков регистров сокетов
#define SOCK_TX_BASE  0x8000 // Базовое смещение для области данных сокета для отправки
#define SOCK_RX_BASE  0xC000 // Базовое смещение для области данных сокета для приема

#define SOCK_MODE_REG         0x0000 // Регистр(смещение в блоке регистров) режима сокета
#define SOCK_CMD_REG          0x0001 // Регистр(смещение в блоке регистров) команды сокета
#define SOCK_INTERRUPT_REG    0x0002 // Регистр(смещение в блоке регистров) прерывания сокета
#define SOCK_STATUS_REG       0x0003 // Регистр(смещение в блоке регистров) состояния сокета
#define SOCK_SRC_PORT_REG     0x0004 // Регистр(смещение в блоке регистров) исходного порта
#define SOCK_DST_IP_REG       0x000C // Регистр(смещение в блоке регистров) целевого IP-адреса
#define SOCK_DST_PORT_REG     0x0010 // Регистр(смещение в блоке регистров) целевого порта
#define SOCK_PROTO_REG        0x0014 // Регистр(смещение в блоке регистров) протокола (используется в режиме IP_RAW)
#define SOCK_TTL_REG          0x0016 // Регистр(смещение в блоке регистров) TTL
#define SOCK_RX_BUF_SIZE_REG  0x001E // Регистр(смещение в блоке регистров) размера буфера на прием 
#define SOCK_TX_BUF_SIZE_REG  0x001F // Регистр(смещение в блоке регистров) размера буфера на передачу
#define SOCK_TX_FREE_REG      0x0020 // Регистр(смещение в блоке регистров) свободного места в буфере отправки сокета 
#define SOCK_TX_READ_PTR_REG  0x0022 // Регистр(смещение в блоке регистров) указателя чтения в буфере отправки сокета 
#define SOCK_TX_WRITE_PTR_REG 0x0024 // Регистр(смещение в блоке регистров) указателя чтения в буфере отправки сокета
#define SOCK_RX_READ_SIZE_REG 0x0026 // Регистр(смещение в блоке регистров) количества принятых байт в буфере приема сокета
#define SOCK_RX_READ_PTR_REG  0x0028 // Регистр(смещение в блоке регистров) указателя чтения в буфере приема сокета

// Протоколы 
#define PROTO_IP            0x00
#define PROTO_ICMP          0x01
#define PROTO_IGMP          0x02
#define PROTO_GGP           0x03
#define PROTO_TCP           0x06
#define PROTO_PUP           0x0C
#define PROTO_UDP           0x11
#define PROTO_IDP           0x16
#define PROTO_ND            0x4D
#define PROTO_RAW           0xFF

// Маски режимов сокета
#define SOCK_MODE_CLOSE     0x00
#define SOCK_MODE_UDP       0x02
#define SOCK_MODE_IPRAW     0x03
#define SOCK_MODE_MACRAW    0x04
#define SOCK_MODE_PPPOE     0x05
#define SOCK_MODE_ND        0x20
#define SOCK_MODE_TCP       0x21
#define SOCK_MODE_MULTI     0x80

// Команды сокета
#define SOCK_CMD_OPEN       0x01 
#define SOCK_CMD_LISTEN     0x02
#define SOCK_CMD_CONNECT    0x04
#define SOCK_CMD_DISCONNECT 0x08
#define SOCK_CMD_CLOSE      0x10
#define SOCK_CMD_SEND       0x20
#define SOCK_CMD_SEND_MAC   0x21
#define SOCK_CMD_SEND_KEEP  0x22
#define SOCK_CMD_RECEIVE    0x40

// Маски прерываний сокета
#define SOCK_INT_CON        0x01
#define SOCK_INT_DISCON     0x02
#define SOCK_INT_RECV       0x04
#define SOCK_INT_TIMEOUT    0x08
#define SOCK_INT_SEND_OK    0x10

// Статусы сокета
#define SOCK_STAT_CLOSED       0x00
#define SOCK_STAT_INIT         0x13
#define SOCK_STAT_LISTEN       0x14
#define SOCK_STAT_SYNSENT      0x15
#define SOCK_STAT_SYNRECV      0x16
#define SOCK_STAT_ESTABILISHED 0x17
#define SOCK_STAT_FIN_WAIT     0x18
#define SOCK_STAT_CLOSING      0x1A
#define SOCK_STAT_TIME_WAIT    0x1B
#define SOCK_STAT_CLOSE_WAIT   0x1C
#define SOCK_STAT_LAST_ACK     0x1D
#define SOCK_STAT_UDP          0x22
#define SOCK_STAT_IPRAW        0x32
#define SOCK_STAT_MACRAW       0x42
#define SOCK_STAT_PPPOE        0x5F
  
#define SOCK_2_ADDR(n, offs) (SOCK_BASE + (n) * SOCK_REG_SIZE + (offs)) // Получает полный адрес по номеру сокета (n - номер сокета)


// ---- DHCP ----
#define DHCP_SERVER_PORT     htons(0x43) //67
#define DHCP_CLIENT_PORT     htons(0x44) //68
#define DHCP_TIMEOUT_MS      6000 // (6 сек) Таймаут DHCP в мс
#define DHCP_RESP_TIMEOUT_MS 4000 // (4 сек) Таймаут ожидания ответа в мс

// Типы DHCP сообщений
#define DHCP_MSG_DISCOVER 1
#define DHCP_MSG_OFFER    2
#define DHCP_MSG_REQUEST  3
#define DHCP_MSG_DECLINE  4
#define DHCP_MSG_ACK      5
#define DHCP_MSG_NAK      6
#define DHCP_MSG_RELEASE  7
#define DHCP_MSG_INFORM   8
#define DHCP_MSG_TIMEOUT  255 // Это сам добавил
 
// Значения полей DHCP-пакета
#define DHCP_PKT_OP_BOOTREQUEST 1                 // Тип операции - запрос
#define DHCP_PKT_OP_BOOTREPLY   2                 // Тип операции - ответ
#define DHCP_PKT_HTYPE          1                 // Тип адреса
#define DHCP_PKT_HLEN           6                 // Длина mac-адреса
#define DHCP_PKT_HOPS           0                 // Число промежуточных маршрутизаторов
#define DHCP_PKT_FLAGS          htons(0x8000)     // Флаг широковещательой операции. Если установлен, то ответ тоже будет широковещательным
#define DHCP_PKT_MAGIC_COOKIE   htonl(0x63825363) // Волшебная кука

// Коды опций DHCP-пакета
#define DHCP_PKT_OPTION_SUBNET   0x01 // 01  - Маска подсети
#define DHCP_PKT_OPTION_GATEWAY  0x03 // 03  - Адрес шлюза
#define DHCP_PKT_OPTION_REQ_IP   0x32 // 50  - Запрашиваемый IP адрес
#define DHCP_PKT_OPTION_MSG_TYPE 0x35 // 53  - Тип сообщения DHCP
#define DHCP_PKT_OPTION_DHCP_IP  0x36 // 54  - Адрес сервера DHCP
#define DHCP_PKT_OPTION_END      0xFF // 255 - Признак конца опций


// ---- ICMP ----
#define PING_TIMEOUT_MS 1000 // Таймаут для пинга (1с)

// Типы ICMP пакетов
#define ICMP_TYPE_REPLY   0 // Ответ
#define ICMP_TYPE_REQUEST 8 // Запрос 
  
// Первый байт в 16-битном слове будет старшим
#define htons(x) ((((uint16_t)(x) & 0xFF00) >> 8) | ((uint16_t)(x) & 0xFF) << 8)

// Первый байт в 16-битном слове будет младшим
#define ntohs(x) htons(x)

// Первый байт в 32-битном слове будет старшим
#define htonl(x) ((((uint32_t)(x) & 0xFF000000) >> 24) | (((uint32_t)(x) & 0x00FF0000) >> 8) | (((uint32_t)(x) & 0x0000FF00) << 8) | (((uint32_t)(x) & 0x000000FF) << 24))

// Первый байт в 32-битном слове будет младшим
#define ntohl(x) htonl(x)


namespace W5500Lite {
 
 uint8_t mac[]    = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x00}; 
 uint32_t localIP = 0;           // Полученный от сервера IP
 uint32_t netMask = 0;           // Маска подсети
 uint32_t gwIP    = 0;           // Адрес шлюза
 uint32_t dhcpIP  = 0xFFFFFFFF;  // Адрес сервера (в начале броадкаст 255.255.255.255)


 // Вывести IP-адрес. 
 /*
  IP 172.16.0.1 в устройстве хранится как AС 10 00 01, big-endian т.е. 172(0xAC) - старший октет.
  В uint32_t little-endian, т.е. значение будет 01 00 10 AC (0x010010AC).
 */ 
 void _print_ip(Print &p, const uint32_t ip){
  p.print(ip & 0xFF);
  p.print(".");
  p.print((ip  >> 8) & 0xFF);
  p.print(".");
  p.print((ip  >> 16) & 0xFF);
  p.print(".");
  p.print((ip  >> 24) & 0xFF);  
 }//_print_ip

 
// ---- Hardware ---- 

 // Прочесть данные по адресу
 uint16_t _read(const uint16_t addr, uint8_t* buf, uint16_t size){
  uint8_t ctrl = 0; // по умолчанию управляющие биты = 0 (выбор блока = 0, режим чтения (0), режим работы - переменная длина (0))
  switch (addr){
   
   // регистры сокетов (от 0x100 до 0x8000)
   case 0x0100 ... 0x7FFF: ctrl = ((addr >> 3) & 0xE0) | 0x08; break; 

   // буферы передачи (от 0x8000 до 0xC000). размер буферов 2048
   case 0x8000 ... 0xBFFF: ctrl = ((addr >> 6) & 0xE0) | 0x10; break;
    
   // буферы приема (от 0xC000 до 0xFFFF). размер буферов 2048
   case 0xC000 ... 0xFFFF: ctrl = ((addr >> 6) & 0xE0) | 0x18; break;
    
   //Управляющие регистры (< 0x0100)
   default: break; // Для младших адресов  так и оставляем 0
  }//switch

  digitalWrite(CS_PIN, LOW); // Прижимаем CS 
  SPI.transfer16(addr); // Адрес отдаем старшим байтом вперед (transfer16 делает это сам)
  SPI.transfer(ctrl); // Байт управления
  memset(buf, 0, size); // Забиваем буфер нулями, т.к. у нас тут прием, а не отправка
  SPI.transfer(buf, size) ; // Отправляем буфер  
  digitalWrite(CS_PIN, HIGH); // Отпускаем CS 

  return size;
 }//read


 // Прочесть регистр (от 1 до 4 байт). 
 // Байты возвращаются в порядке их хранения, т.е. little-endian. Так при чтении 0x78, 0x56, 0x34 и 0x12, значение res будет 0x12345678
 uint32_t _read_reg(const uint16_t addr, uint8_t bytes = 1) {
  uint32_t res = 0;
  bytes = constrain(bytes, 1, 4);
  _read(addr, (uint8_t*)&res, bytes); 
  return res;
 }//_read_reg


 // Отправить данные по адресу 
 uint16_t _write(const uint16_t addr, const uint8_t* buf, uint16_t size){
  uint8_t ctrl = 0x04; // по умолчанию управляющие биты = 0x04 (выбор блока = 0, режим чтения (1 = запись), режим работы - переменная длина (0))  
  switch (addr){
   
   // регистры сокетов (от 0x100 до 0x8000)
   case 0x0100 ... 0x7FFF: ctrl = ((addr >> 3) & 0xE0) | 0x0C; break; 

   // буферы передачи (от 0x8000 до 0xC00). размер буферов 2048
   case 0x8000 ... 0xBFFF: ctrl = ((addr >> 6) & 0xE0) | 0x14; break;
    
   // буферы приема (от 0xC000 до 0xFFFF). размер буферов 2048
   case 0xC000 ... 0xFFFF: ctrl = ((addr >> 6) & 0xE0) | 0x1C; break;
    
   //Управляющие регистры (< 0x0100)
   default: break; // Для младших адресов  так и оставляем 0x04
  }//switch
  
  digitalWrite(CS_PIN, LOW); // Прижимаем CS 
  SPI.transfer16(addr); // Адрес отдаем старшим байтом вперед (transfer16 делает это сам)
  SPI.transfer(ctrl); // Байт управления
  // Т.к. SPI затрет данные буфера при передаче, то будем передавать побайтно
  for(uint16_t i = 0; i < size; i++) SPI.transfer(buf[i]); //SPI.transfer(buf, size) ;
  digitalWrite(CS_PIN, HIGH); // Отпускаем CS 
  return size; 
 }//_write


 // Записать значение (от 1 до 4 байт) в регистр. 
 // Байты пишутся в порядке их хранения, т.е. little-endian. Так при data = 0x12345678, будут передаваться 0x78, 0x56, 0x34 и 0x12 
 void _write_reg(const uint16_t addr, const uint32_t data, uint8_t bytes = 1){
  bytes = constrain(bytes, 1, 4);
  _write(addr, (uint8_t*)&data, bytes);
 }//_write_reg


 // Софтверный сброс
 uint8_t _reset(){
  _write_reg(MR_REG, 0x80); // Старший бит -> команда сброса
  for(uint8_t i = 0; i < 20; i++){
    if(!_read_reg(MR_REG)) return 1; // Читем MR пока не вернет 0, но не больше 20 раз
    delay(1); //с задержкой 1 ms
  }//for
  return 0;
 }//_reset


 // Инициализация 
 uint8_t _init(){
  pinMode(CS_PIN, OUTPUT);

  // Даем чипу очухаться
  delay(560);
  digitalWrite(CS_PIN, LOW); // Прижимаем CS
  SPI.beginTransaction(SPI_CONFIG);

  // Сброс 
  if(!_reset()) return 0; // Сброс неудался

  // Инициализируем
  _write_reg(MR_REG, 0x08); // Включить PPPoE
  if(_read_reg(MR_REG) != 0x08) return 0; // Если не включился, то облом

  _write_reg(MR_REG, 0x10); // Включить блокировку ping
  if(_read_reg(MR_REG) != 0x10) return 0; // Если не включилась, то облом

  _write_reg(MR_REG, 0x00); // Выключить все
  if(_read_reg(MR_REG)) return 0; // Если не выключилось, то облом

  if(_read_reg(VERSION_REG) != VERSION_VAL) return 0; // Не та версия чипа

/*
  // Устанавливаем размеры буферов для сокетов (можно и не ставить. они и так по умолчанию 2кб)
  for(uint8_t i = 0; i < SOCK_NUM; i++){
    _write_reg(SOCK_2_ADDR(i, RX_BUF_SIZE_REG), SOCK_SIZE >> 10);
    _write_reg(SOCK_2_ADDR(i, TX_BUF_SIZE_REG), SOCK_SIZE >> 10);
  }//for
*/  
  // Адреса
  _write(SRC_MAC_REG, mac, 6); // Прописываем MAC
  _write_reg(SRC_IP_REG, 0, 4); // Прописываем нулевой IP (0.0.0.0)
  
  SPI.endTransaction();
  return 1; // Все гуд
 }//_init


 // Есть ли физическое соединение
 uint8_t _is_link_on(){
  return _read_reg(PHYCFG_REG) & 0x01;
 }//_is_link_on



 // ---- Сокеты ----
 // Отправить сокету команду 
 void _sock_cmd(const uint8_t sock_num, const uint8_t sock_cmd){
  _write_reg(SOCK_2_ADDR(sock_num, SOCK_CMD_REG), sock_cmd);
  while(_read_reg(SOCK_2_ADDR(sock_num, SOCK_CMD_REG)));
 }//_sock_cmd


 // Открыть сокет (вернет номер или -1 если не нашел свободного. Сам закрывать не пыается)  
 int8_t _sock_open(const uint8_t mode, const uint16_t port, const uint8_t proto = 0){
  SPI.beginTransaction(SPI_CONFIG);
  for(int8_t sock = 0; sock < SOCK_NUM; sock++){ // Ищем свободнй сокет
    if(_read_reg(SOCK_2_ADDR(sock, SOCK_STATUS_REG)) == SOCK_STAT_CLOSED){ // Читаем статус. Свободен ли
      // Свободен
      _write_reg(SOCK_2_ADDR(sock, SOCK_MODE_REG), mode); // В регистр режима пишем режим работы сокета
      _write_reg(SOCK_2_ADDR(sock, SOCK_INTERRUPT_REG), 0xFF); // В регистр прерываний пишем 0xFF
      _write_reg(SOCK_2_ADDR(sock, SOCK_SRC_PORT_REG), port, 2); // В регистр исходного порта - порт (2 байта). Помним про htons в константе.
      if(mode == SOCK_MODE_IPRAW) _write_reg(SOCK_2_ADDR(sock, SOCK_PROTO_REG), proto); // В регистр протокола пишем только если IP_RAW
      _sock_cmd(sock, SOCK_CMD_OPEN); // Даем команду на открытие сокета
      SPI.endTransaction();
      return sock;
    }//if
  }//for
  SPI.endTransaction();
  return -1; // Свободный сокет не нашли
 }//_sock_open 
 

 // Закрыть сокет
 void _sock_close(const int8_t sock){
  SPI.beginTransaction(SPI_CONFIG);
  _sock_cmd(sock, SOCK_CMD_CLOSE);
  SPI.endTransaction();
 }//_sock_close


 // Отправить данные 
 uint8_t _sock_send(const uint8_t sock){
  SPI.beginTransaction(SPI_CONFIG);
  // Даем команду отправить данные
  _sock_cmd(sock, SOCK_CMD_SEND);

  // Ждем, пока не отправится
  uint8_t res;
  do {
    res = _read_reg(SOCK_2_ADDR(sock, SOCK_INTERRUPT_REG)); // Читаем регистр прерываний сокета
    if(res & SOCK_INT_TIMEOUT){ // Таймаут
      _write_reg(SOCK_2_ADDR(sock, SOCK_INTERRUPT_REG), SOCK_INT_SEND_OK | SOCK_INT_TIMEOUT);
      SPI.endTransaction();
      return false; 
    }//if
    SPI.endTransaction();
    yield();
    SPI.beginTransaction(SPI_CONFIG);    
  } while((res & SOCK_INT_SEND_OK) != SOCK_INT_SEND_OK);

  // Все прошло нормально
  _write_reg(SOCK_2_ADDR(sock, SOCK_INTERRUPT_REG), SOCK_INT_SEND_OK); 
  SPI.endTransaction();
  return true;  
 }//_sock_send


 // Записать в буфер передачи сокета. SPI должен быть уже настроен (beginTranscation)
 uint16_t _sock_write(const int8_t sock, uint16_t offset, uint8_t * buf, uint16_t size){
  // Поджимаем размер, чтобы не пытаться отправить больше, чем есть свободного места в буфере TX_FSR -> SOCK_TX_FREE_REG
  uint16_t free_size; // Читаем подряд, пока значения не устаканятся
  do{
    free_size = _read_reg(SOCK_2_ADDR(sock, SOCK_TX_FREE_REG), 2);
  } while(free_size != _read_reg(SOCK_2_ADDR(sock, SOCK_TX_FREE_REG), 2));
  size = min(size, htons(free_size)); // big-endian! htons!

  // Читаем текущий указатель TX_WR -> SOCK_TX_WRITE_PTR_REG
  uint16_t ptr = _read_reg(SOCK_2_ADDR(sock, SOCK_TX_WRITE_PTR_REG), 2); 
  
  // Добавляем к нему смещение offset. получаем ptr. Убираем старшие биты, что б не было больше 2048 (0x800)
  ptr = (offset + htons(ptr)) & (SOCK_SIZE - 1); // big-endian htons!

  // Полный адрес: полное смещение +  базовый адрес буфера 0x8000 + sock * 2048 
  // Если размер данных + полное смещение больше размера сокета, то отправляем частями
  if((ptr + size) > SOCK_SIZE) {
    uint16_t part = SOCK_SIZE - ptr; // Сколько влезет до конца сокета
    _write(SOCK_TX_BASE + (SOCK_SIZE * sock) + ptr, buf, part); // Пишем сколько влезет от текущего места
    _write(SOCK_TX_BASE + (SOCK_SIZE * sock), buf + part, size - part); // Пишем оставшееся с самого начала буфера
  }//if
  else 
    _write(SOCK_TX_BASE + (SOCK_SIZE * sock) + ptr, buf, size); // Пишем целиком

  // Пишем в TX_WR (SOCK_TX_WRITE_PTR_REG) новый указатель (старый + сколько записали). big-endian htons!
  _write_reg(SOCK_2_ADDR(sock, SOCK_TX_WRITE_PTR_REG), htons(ptr + size), 2); 

  // Возвращаем сколько записали
  return size;
 }//_sock_write


 // Возвращает количество принятых байт в буфере сокета
 uint16_t _sock_available(const int8_t sock){
  uint16_t data_size; // Читаем подряд, пока значения не устаканятся
  SPI.beginTransaction(SPI_CONFIG);
  do{
    data_size = _read_reg(SOCK_2_ADDR(sock, SOCK_RX_READ_SIZE_REG), 2);
  } while(data_size != _read_reg(SOCK_2_ADDR(sock, SOCK_RX_READ_SIZE_REG), 2));
  SPI.endTransaction();
  return htons(data_size); // big-endian! htons! 
 }// _sock_available


 // Прочесть из буфера приема сокета. SPI должен быть уже настроен (beginTranscation)
 uint16_t _sock_read(const int8_t sock, uint8_t * buf, uint16_t size){
  
  if(!size) return 0; // Если длина = 0, то ничего не делаем
  
  // Читаем текущий указатель RX_RD -> SOCK_RX_READ_PTR_REG
  uint16_t ptr = _read_reg(SOCK_2_ADDR(sock, SOCK_RX_READ_PTR_REG), 2); 
  
  // Добавляем к нему смещение offset. получаем ptr. Убираем старшие биты, что б не было больше 2048 (0x800)
  ptr = htons(ptr) & (SOCK_SIZE - 1); // big-endian htons!
  
  // Полный адрес: полное смещение +  базовый адрес буфера 0xC000 + sock * 2048
  // Читаем в буфер данные, начиная с указателя
  if(buf){ // Если читаем в NULL, то само чтение не выполняем
    if((ptr + size) > SOCK_SIZE) { // Если размер данных + полное смещение больше размера сокета, то читаем частями
      uint16_t part = SOCK_SIZE - ptr; // Сколько есть до конца сокета
      _read(SOCK_RX_BASE + (SOCK_SIZE * sock) + ptr, buf, part); // Читаем сколько есть от текущего места
      _read(SOCK_RX_BASE + (SOCK_SIZE * sock), buf + part, size - part); // Читаем оставшееся с самого начала буфера
    }//if
    else 
      _read(SOCK_RX_BASE + (SOCK_SIZE * sock) + ptr, buf, size); // Читаем целиком
  }//if buf    
  
  // Обновить указатель RX_RD
  // Пишем в RX_RD (SOCK_RX_READ_PTR_REG) новый указатель (старый + сколько прочли). big-endian htons!
  _write_reg(SOCK_2_ADDR(sock, SOCK_RX_READ_PTR_REG), htons(ptr + size), 2); 

  // Отправить команду RECV
  _sock_cmd(sock, SOCK_CMD_RECEIVE);
  return size;
 }// _sock_read

  
// Побыстрому читаем 1 байт из буфера приема сокета. SPI должен быть уже настроен (beginTranscation)
 uint8_t _sock_read(const int8_t sock){
  uint8_t res;
  _sock_read(sock, &res, 1);
  return res;
 }//_sock_read
  
   
 // Очистить буфер приема сокета. SPI должен быть уже настроен (beginTranscation)
 void _sock_flush(const int8_t sock){
  uint16_t size = _sock_available(sock);
  if(!size) return; // Пусто. Ничего чистить не надо
  SPI.beginTransaction(SPI_CONFIG);
  _sock_read(sock, (uint8_t*)NULL, size);
  SPI.endTransaction();
 }// _sock_flush

 

 // ---- DHCP ----
 // Фазы обмена с DHCP сервером
 typedef enum {
  START,
  DISCOVER,
  REQUEST,
  LEASED,
  PREREQUEST,  
  RELEASE
 } DHCPState; 

 
 // Данные для общения с DHCP-сервером
 typedef struct {
  int8_t sock;       // Номер сокета
  uint32_t trans_id; // Идентификатор транзакции
  uint32_t t_start;  // Момент начала общения с сервером (в millis())
 } DHCPData;


 // Начало DHCP-пакета
 typedef struct {
  uint8_t op;     // Тип операции
  uint8_t hType;  // Тип адреса
  uint8_t hLen;   // Длина mac-адреса
  uint8_t hops;   // Число промежуточных маршрутизаторов
  uint32_t id;    // Идентификатор транзакции
  uint16_t secs;  // Время в секундах с момента начала процесса получения адреса
  uint16_t flags; // Флаги
 } dhcpPktBegin;


// Блок IP-адресов в dhcp-пакете
typedef struct {
  uint32_t ciaddr; // Адрес клиента (при обновлении адреса)
  uint32_t yiaddr; // Предложенный сервером новый адрес клиента
  uint32_t siaddr; // Адрес следующего сервера в цепочке сереров
  uint32_t giaddr; // Адрес агента ретрансляции
 } dhcpPktIP;


// Опции DHCP-пакета
typedef struct {
  uint8_t code; // Код опции
  uint8_t len;  // Длина области даных опции
  uint8_t data; // Данные опции
  uint8_t align;
 } dhcpPktOptionByte;

// Опциия DHCP-пакета содержащая IP-адрес
typedef struct {
  uint8_t code; // Код опции
  uint8_t len;  // Длина области даных опции
  uint32_t ip;  // Данные опции (IP-адрес)
 } dhcpPktOptionIP;
 

// Заголовок UDP 
typedef struct {
  uint32_t remoteIP; // IP адрес источника
  uint16_t remotePort; // Порт источника
  uint16_t dataSize; // Размер полезной нагрузки пакета
 } dhcpUdpHdr;


 // Разобрать ответ DHCP-сервера. Возвращает тип сообщения. Если таймаут: 255. Если все плохо: 0. 
 // Устанавливает глобальные netMask, gwIP, dhcpIP и localIP в случае удачи.
 // Принимает параметры обмена в структура data
 uint8_t _dhcp_parse(const DHCPData data){
  uint8_t buf[16]; // Рабочий буфер
  memset(buf, 0, sizeof(buf)); // Обнуляем
  uint8_t msg_type = 0; // Тип пришедшего от DHCP-сервера сообщения 
  uint16_t r_size; // Количество полученных байт в буфере сокета

  // Ждем ответа до таймаута
  uint32_t t_start = millis();
  while(!(r_size = _sock_available(data.sock))){ 
    if((millis() - t_start) > DHCP_RESP_TIMEOUT_MS) return DHCP_MSG_TIMEOUT;
    delay(50);
  }//while

  SPI.beginTransaction(SPI_CONFIG);

  // Читаем udp-заголовок
  dhcpUdpHdr *hdr = (dhcpUdpHdr*)buf; // совмещаем структуру заголовка с буфером
  _sock_read(data.sock, buf, sizeof(dhcpUdpHdr)); 
  
  // Проверяем порт сервера
  if(hdr->remotePort != DHCP_SERVER_PORT){
    _sock_flush(data.sock);
    return 0;
  }//if

  // Читаем DHCP заголовок
  dhcpPktBegin *pkt = (dhcpPktBegin*)buf; // совмещаем структуру с буфером
  _sock_read(data.sock, buf, sizeof(dhcpPktBegin)); 

  // Проверяем id транзакции и BOOTREPLY
  if((pkt->op != DHCP_PKT_OP_BOOTREPLY) || (pkt->id != data.trans_id)){
     _sock_flush(data.sock);
     return 0;
  }//if

  // Получаем предложенный сервером IP
  dhcpPktIP *ip = (dhcpPktIP*)buf; // совмещаем структуру заголовка с буфером
  _sock_read(data.sock, buf, sizeof(dhcpPktIP)); 
  localIP = ip->yiaddr;

  // Проматываем до магической куки. 16 байт mac + 64 байта sname + 128 байт BOOTP = 208 байт 
  _sock_read(data.sock, (uint8_t*)NULL, 208); 

  // Читаем Магическую куку
  _sock_read(data.sock, buf, 4); 
  if(*(uint32_t*)buf != DHCP_PKT_MAGIC_COOKIE){
    _sock_flush(data.sock);
   return 0;
  }//if

  // Разгребаем опции 
  uint8_t opt_size = 0; // Длина опции
  while(_sock_available(data.sock)){
    switch(_sock_read(data.sock)){ // Читаем тип опции и смотрим, что делать дальше

      case DHCP_PKT_OPTION_SUBNET: // Подсеть
        opt_size = _sock_read(data.sock); // Длина опции
        _sock_read(data.sock, (uint8_t*)&netMask, 4); // Читаем маску подсети
        _sock_read(data.sock, (uint8_t*)NULL, opt_size - 4); // Подчищаем
      break;      

      case DHCP_PKT_OPTION_GATEWAY: // Шлюз
        opt_size = _sock_read(data.sock); // Длина опции
        _sock_read(data.sock, (uint8_t*)&gwIP, 4); // Читаем адрес шлюза
        _sock_read(data.sock, (uint8_t*)NULL, opt_size - 4); // Подчищаем
      break;      

      case DHCP_PKT_OPTION_MSG_TYPE: // Тип сообщения DHCP (DHCP_MSG_)
        opt_size = _sock_read(data.sock); // Читаем и выкидываем длину. Она тут всегда = 1
        msg_type = _sock_read(data.sock);
        _sock_read(data.sock, (uint8_t*)NULL, opt_size - 1); // Подчищаем
      break;

      case DHCP_PKT_OPTION_DHCP_IP: // DHCP-сервер
        opt_size = _sock_read(data.sock); // Длина опции
        _sock_read(data.sock, (uint8_t*)&dhcpIP, 4); // Читаем адрес DHCP
        _sock_read(data.sock, (uint8_t*)NULL, opt_size - 4); // Подчищаем
      break;      

      case DHCP_PKT_OPTION_END: // Конец опций
        _sock_flush(data.sock); // Очищаем буфер
      break;      
      
      default:  // Необслуживаемая опция
        opt_size = _sock_read(data.sock); // Длина опции
        _sock_read(data.sock, (uint8_t*)NULL, opt_size); // Просто проматываем
      break;
    }//switch
  }//while

  // Очищаем, что осталось в буфере
  _sock_flush(data.sock);
  
  SPI.endTransaction();
  return msg_type;
 }//_dhcp_parse


 // Отправить запрос DHCP-серверу. Вернет 0, если все плохо
 // Принимает тип сообщения и параметры обмена в структура data)
 // Время, прошедшее с начала обмена считает сам 
 uint8_t _dhcp_send(const uint8_t msg_type, const DHCPData data){
  uint8_t buf[16]; // Рабочий буфер
  memset(buf, 0, sizeof(buf)); // Обнуляем
  uint16_t offset = 0; // Смещение в буфере сокета
      
  dhcpPktBegin *pkt = (dhcpPktBegin*)buf; // совмещаем структуру с буфером
  
  // Начинаем формировать UDP-пакет
  SPI.beginTransaction(SPI_CONFIG);
  _write_reg(SOCK_2_ADDR(data.sock, SOCK_DST_IP_REG), 0xFFFFFFFF, 4); // В регистр целевого IP - 255.255.255.255 (4 байта)
  _write_reg(SOCK_2_ADDR(data.sock, SOCK_DST_PORT_REG), DHCP_SERVER_PORT, 2); // В регистр целевого порта - порт (2 байта). big-endian htons! 

  // Заполняем поля
  pkt -> op = DHCP_PKT_OP_BOOTREQUEST;
  pkt -> hType = DHCP_PKT_HTYPE;
  pkt -> hLen = DHCP_PKT_HLEN;
  //pkt -> hops; не надо. и так = 0
  pkt -> secs = (millis() - data.t_start) / 1000; // Секунды с начала обмена
  pkt -> id = data.trans_id; // id транзакции
  pkt -> flags = DHCP_PKT_FLAGS; // broadcast

  // Отпрвляем в буфер сокета
  _sock_write(data.sock, offset, buf, sizeof(dhcpPktBegin)); 
  offset += sizeof(dhcpPktBegin); 

  // Пишем ip-шники  
  memset(buf, 0, sizeof(buf)); // Обнуляем
  _sock_write(data.sock, offset, buf, sizeof(buf)); // 16-байт: curIP, newIP, srvIP, giIP
  offset += sizeof(buf);

  // Пишем mac-адрес с расширением
  memset(buf, 0, sizeof(buf)); // Обнуляем
  memcpy(buf, mac, sizeof(mac)); // Копируем MAC
  _sock_write(data.sock, offset, buf, sizeof(buf)); // Мак + добивание до 16 байт
  offset += sizeof(buf);
  
  // Пишем имя сервера и файла на сервере (BOOTP. У нас все нулевое). 192 байта. 12 циклов 16-байтного буфера 
  memset(buf, 0, sizeof(buf)); // Обнуляем
  for(uint8_t i = 0; i < 12; i++, offset += sizeof(buf)) _sock_write(data.sock, offset, buf, sizeof(buf)); 

  // Магическая кука
  memset(buf, 0, sizeof(buf)); // Обнуляем
  *(uint32_t*)buf = DHCP_PKT_MAGIC_COOKIE;
  _sock_write(data.sock, offset, buf, sizeof(DHCP_PKT_MAGIC_COOKIE)); // 4-байта
  offset += sizeof(DHCP_PKT_MAGIC_COOKIE);
  //240
     
  // Опции. На discover надо опцию поиска сервера. (53 со значением 1), а на REQUEST 53 со значением 3 
  memset(buf, 0, sizeof(buf)); // Обнуляем
  uint8_t opt_offs = 0; // Смещение в буфере для опций
  dhcpPktOptionByte *opt = (dhcpPktOptionByte*)&(buf[opt_offs]); // совмещаем структуру опции с началом буфера
  opt -> code = DHCP_PKT_OPTION_MSG_TYPE; // Опция типа сообщения 
  opt -> len = 0x01; // Длина опции
  opt -> data = msg_type; // DHCP_MSG_DISCOVER или DHCP_MSG_REQUEST
  //opt -> align = 0; // Выравнивание тут и так ноль  
  opt_offs += sizeof(dhcpPktOptionByte); // 4 байта на эту опцию

  // Для REQUEST-а надо еще добавить опции нашего ip и ip сервера
  if(msg_type == DHCP_MSG_REQUEST){
    dhcpPktOptionIP *addr_opt = (dhcpPktOptionIP*)&(buf[opt_offs]); // совмещаем структуру опции с началом буфера  

    addr_opt -> code = DHCP_PKT_OPTION_REQ_IP; // Запрашиваемый IP
    addr_opt -> len = 0x04; // 4 байта
    addr_opt -> ip = localIP; // Наш адрес
    opt_offs += sizeof(dhcpPktOptionIP); // Длина опции 6

    addr_opt = (dhcpPktOptionIP*)&(buf[opt_offs]); // совмещаем структуру опции с началом буфера  
    addr_opt -> code = DHCP_PKT_OPTION_DHCP_IP; // Адрес DHCP-сервера
    addr_opt -> len = 0x04; // 4 байта
    addr_opt -> ip = dhcpIP; // Адрес DHCP сервера
    opt_offs += sizeof(dhcpPktOptionIP); // Длина опции 6    
  }//if
  
  buf[opt_offs] = DHCP_PKT_OPTION_END; // Ставим признак конца опций
  opt_offs += 1; // 1 байт на эту опцию  
  _sock_write(data.sock, offset, buf, opt_offs); 
    
  SPI.endTransaction();
  
  // Отправляем
  return _sock_send(data.sock);    
 }//_dhcp_send

 
 // Получить (и установить) настройки через dhcp. Вернуть 0 если облом.
 // Получение однократное. Без продления и использования времени аренды.
 // Пишет в SRC_IP_REG полученный IP в случае удачи, а так же маску и шлюз MSK_REG и GW_REG

/* 
  Результаты пишем в глобальные 
  uint32_t localIP
  uint32_t netMask
  uint32_t gwIP  
  uint32_t dhcpIP
*/  
 uint8_t _dhcp_request(){
  uint8_t res = 0; 
  DHCPState state = START;
  DHCPData data; 
 
  data.sock = _sock_open(SOCK_MODE_UDP, DHCP_CLIENT_PORT); // Открываем сокет

  if(data.sock < 0) return res; // Нет свободных сокетов

  data.trans_id = analogRead(P5); // Формируем идентификатор транзакции
  data.t_start = millis(); // Время начала запросов
  
  while((state != LEASED) && ((millis() - data.t_start) < DHCP_TIMEOUT_MS)){ // Крутимся пока не получим настройки или не выйдет таймаут
    switch(state){
      case START: //Отправляем запрос DISCOVER и переходим в состояние DISCOVER
        _dhcp_send(DHCP_MSG_DISCOVER, data);
        state = DISCOVER;
      break;

      case PREREQUEST: //Отправляем запрос REQUEST и переходим в состояние REQUEST
        _dhcp_send(DHCP_MSG_REQUEST, data);
        state = REQUEST;
      break;

      case DISCOVER: // Разбираем ответ
        res = _dhcp_parse(data);
        if(res == DHCP_MSG_OFFER) state = PREREQUEST; // Если mesType = OFFER, то преходим в состояние PREREQUEST
      break;

      case REQUEST: // Разбираем ответ      
        res = _dhcp_parse(data);
        state = (res == DHCP_MSG_ACK)? LEASED : START; // Если ACK то все OK. Иначе начинаем сначала.
      break;
      
      default: break;
    }//switch
  }//while

  // Закрываем сокет
  _sock_close(data.sock);

  // Прописываем полученный IP, маску и шлюз
  if(res == DHCP_MSG_ACK){
    _write_reg(SRC_IP_REG, localIP, 4); // Прописываем полученный IP
    _write_reg(GW_REG, gwIP, 4); // Прописываем полученный шлюз
    _write_reg(MSK_REG, netMask, 4); // Прописываем полученную маску
  }//if
  
  return res; // Возвращаем результат
 }//_dhcp_request



// ---- ICMP ----
 // ICMP-пакет
 typedef struct {
  uint8_t  type; // Тип пакета (8 - запрос, 0 - ответ)
  uint8_t  code; // Код пакета (0 для запроса и ответа)
  uint16_t cSum; // Контрольная сумма пакета
  uint16_t id;   // Идентификатор пакета
  uint16_t seq;  // Порядковый номер пакета
 } icmpPkt;


 // Считаем контрольную сумму для ICMP
 uint16_t _icmp_cSum(uint8_t * data, uint16_t size){
  uint32_t sum = 0;
  // Считаем арифметическую сумму по словам
  while(size >= 2){
    sum += ((uint16_t)*data << 8) | *(data + 1);
    data += 2;
    size -= 2;
  }//while

  // Если длина буфера нечетная (после цикла осталось > 0), дополняем нулем
  if(size) sum += (uint16_t)*data << 8;

  // Пока сумма не влезет в 2 байта, складываем старшее слово с младшим
  while(sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);

  // Конвертируем в big-endian и берем дополнение
  return ~htons((uint16_t)sum);
 } //_icmp_cSum

 
 // Отправляет пинг на заданный адрес и ждет ответа до PING_TIMEOUT_MS. Возвращает -1, если ошибки. Иначе возаращает время в us!.
 // Если был таймаут, то возвращает PING_TIMEOUT_MS * 1000
 // Свой IP и MAC уже прописаны в устройство. mac - init-ом, а ip - dhcp_request-ом
 // * dst_ip - адрес, на который отправляем запросы
 // * seq - номер последовательности
 // * ttl - возвращает TTL
 int32_t _ping(const uint32_t dst_ip, const uint16_t seq, uint8_t &ttl){
  int8_t sock = _sock_open(SOCK_MODE_IPRAW, 0, PROTO_ICMP);
  if(sock < 0) return -1; // Нет свободных сокетов

  // Готовим ICMP запрос
  SPI.beginTransaction(SPI_CONFIG);
  _write_reg(SOCK_2_ADDR(sock, SOCK_DST_IP_REG), dst_ip, 4); // Регистр целевого IP
  _write_reg(SOCK_2_ADDR(sock, SOCK_DST_PORT_REG), 0, 2); // Регистр целевого порта - 0
  _write_reg(SOCK_2_ADDR(sock, SOCK_TTL_REG), 128); // Регистр TTL

  icmpPkt pkt;
  pkt.type = ICMP_TYPE_REQUEST; // Тип пакета
  pkt.code = 0; // Код для запроса и ответа = 0
  pkt.cSum = 0; // Контольная сумма должна быть сброшена перед рассчетом
  pkt.id = 1;
  pkt.seq = htons(seq); //big-endian htons!
  pkt.cSum = _icmp_cSum((uint8_t*)&pkt, sizeof(pkt)); 
  _sock_write(sock, 0, (uint8_t*)&pkt, sizeof(pkt)); 
  SPI.endTransaction();

  // Отправляем
  if(! _sock_send(sock)){ 
    _sock_close(sock);
    return -1;
  }//if

  // Ждем ответа до таймаута
  uint16_t r_size; // Количество полученных байт в буфере сокета
  //uint32_t t_start = millis();
  uint32_t t_start = micros();
  while(!(r_size = _sock_available(sock))){ 
/*    
    if((millis() - t_start) > PING_TIMEOUT_MS){ 
      _sock_close(sock);
      return PING_TIMEOUT_MS;
    }//if   
*/    
    if((micros() - t_start) > ((uint32_t)PING_TIMEOUT_MS) * 1000){ 
      _sock_close(sock);
      return ((int32_t)PING_TIMEOUT_MS) * 1000;
    }//if

  }//while
  
  // Разбираем пришедший пакет
  uint32_t remoteIP;
  SPI.beginTransaction(SPI_CONFIG);

  // Проверяем адрес, с которого пришел ответ
  _sock_read(sock, (uint8_t*)&remoteIP, 4); 
  if(remoteIP != dst_ip){ // Не с того IP пришел пакет
    _sock_flush(sock);
    _sock_close(sock);
    return -1;
  }//if

  // Проматываем лишнее (2 байта)
  _sock_read(sock, (uint8_t*)NULL, 2);

  // Читаем ICMP-пакет
  _sock_read(sock, (uint8_t*)&pkt, sizeof(pkt));

  uint16_t sum = pkt.cSum;
  pkt.cSum = 0;
  if(sum != _icmp_cSum((uint8_t*)&pkt, sizeof(pkt))){ // Не та контрольная сумма
    _sock_flush(sock);
    _sock_close(sock);
    return -1;
  }//if

  if(pkt.type != ICMP_TYPE_REPLY){ // Не тот тип
    _sock_flush(sock);
    _sock_close(sock);
    return -1;
  }//if

  if(pkt.code){ // код должен быть == 0
    _sock_flush(sock);
    _sock_close(sock);
    return -1;
  }//if

  if(pkt.seq != htons(seq)){ // Не тот номер последовательности //big-endian htons!
    _sock_flush(sock);
    _sock_close(sock);
    return -1;
  }//if

  // Читаем TTL
  ttl = _read_reg(SOCK_2_ADDR(sock, SOCK_TTL_REG));
  
  SPI.endTransaction();  
  _sock_close(sock);// Закрываем сокет

  //return (int16_t)(millis() - t_start);
  return (int32_t)(micros() - t_start);
 }// _ping
 
} //namespace


using namespace W5500Lite;


/*
 * Подчистить по пингам и сделать юзерскую интерфейсную часть!!!!!!!!!!!!!!!!!!!!!!!!!
*/


// == Main plugin function ==
void plgW5500Lite(){
  // Init
  SPI.begin();


  Serial.print("Init: ");
  Serial.println(_init());
  
  Serial.print("DHCP: ");
  Serial.println(_dhcp_request());
  Serial.print("IP: ");
  _print_ip(Serial, localIP);
  Serial.print("\nMask: ");
  _print_ip(Serial, netMask);
  Serial.print("\nGateway: ");
  _print_ip(Serial, gwIP);
  Serial.print("\nDHCP: ");
  _print_ip(Serial, dhcpIP);
  Serial.println();


  //uint32_t pingIP = 0x150010AC;
  uint32_t pingIP = 0x0100A8C0; // 192.168.0.1
  //uint32_t pingIP = 0xF2FFFF05; // 5.255.255.242
   _print_ip(Serial, pingIP);
  Serial.println();
  
  uint8_t ttl = 0;
  uint32_t time_us;
  //int16_t time_ms = _ping(gwIP, seq, ttl); //0x150010AC
/*  
  time_ms = _ping(pingIP, 0, ttl); //0x150010AC - 172.16.0.21
  Serial.print("Ping seq: ");
  Serial.println(seq);
  Serial.print("Ping ttl: ");
  Serial.println(ttl);
  Serial.print("Ping time: ");
  Serial.println(time_ms);
*/  
  for(uint8_t seq = 1; seq <= 4; seq ++){
    time_us = _ping(pingIP, seq, ttl);
    Serial.print(seq);
    Serial.print(" ");
    Serial.print(ttl);
    Serial.print(" ");
    Serial.println(time_us);
    delay(500);
  }//for

  

/*
  Ethernet.init(CS_PIN); //только ставит CS и все
  Ethernet.begin(mac, 6000); // инитит чип, прописывает мак и 0 IP, и запускает dhcp и прписывает полученные от него данные  

  if(Ethernet.hardwareStatus() == EthernetNoHardware) Serial.println("No hardware");
  if(Ethernet.linkStatus() == LinkOFF) Serial.println("No Link");
  Serial.println(Ethernet.localIP());
  core.moveCursor(0, 1);
  core.println(Ethernet.localIP());
*/

  SPI.beginTransaction(SPI_CONFIG);
  
  //_write_reg(0x0000, 0xAABBCCDD, 4);01 00 10 AC
  //_write_reg(0x0000, 0x010010AC, 4);
  //_write_reg(0x0000, 0xAABB, 2);
  //Serial.println(_read_reg(VERSION_REG), HEX);
  //Serial.println(_read_reg(PHYCFG_REG), HEX);
  //Serial.println(_read_reg(0x0001, 4), HEX); // Gateway
/*
  uint8_t macc[6];
  _read(SRC_MAC_REG, macc, 6);
  core.printHexArray(Serial, macc, 6);
  Serial.println();
  
  uint32_t ip = _read_reg(SRC_IP_REG, 4);
  Serial.println(ip, HEX);
  _print_ip(Serial, ip);
  //_print_ip(Serial, 0x010010AC);
  Serial.println();
*/  
  SPI.endTransaction();
  



  // Load settings from EEPROM 
    
  // Display Init
  
  // Main loop
  while(1){
    delay(500);
  }//while
}//plgW5500Lite
