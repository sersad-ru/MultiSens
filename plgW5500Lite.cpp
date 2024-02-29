#include "plgW5500Lite.h"
#include <SPI.h>
#include <Ethernet.h>

#define CS_PIN   P0
#define MOSI_PIN P2
#define MISO_PIN P3
#define SCK_PIN  P4


#define SPI_CONFIG SPISettings(14000000, MSBFIRST, SPI_MODE0)

/*
 * IP 172.16.0.1 в uint32_t хранится как AС 10 00 01, т.е. 172 - младший октет и 
 * выводиться будет как  01 00 10 AC т.е. 172 - младший значащий байт
*/

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


// ---- Socket ----
#define SOCK_MODE_REG        0x0000 // Регистр(смещение в блоке регистров) режима сокета
#define SOCK_CMD_REG         0x0001 // Регистр(смещение в блоке регистров) команды сокета
#define SOCK_INTERRUPT_REG   0x0002 // Регистр(смещение в блоке регистров) прерывания сокета
#define SOCK_STATUS_REG      0x0003 // Регистр(смещение в блоке регистров) состояния сокета
#define SOCK_SRC_PORT_REG    0x0004 // Регистр(смещение в блоке регистров) исходного порта
#define SOCK_DST_IP_REG      0x000C // Регистр(смещение в блоке регистров) целевого IP-адреса
#define SOCK_DST_PORT_REG    0x0010 // Регистр(смещение в блоке регистров) целевого порта
#define SOCK_RX_BUF_SIZE_REG 0x001E // Регистр(смещение в блоке регистров) размера буфера на прием 
#define SOCK_TX_BUF_SIZE_REG 0x001F // Регистр(смещение в блоке регистров) размера буфера на передачу

#define VERSION_VAL   0x04   // Версия чипа (для W5500 должна быть = 4)
#define SOCK_NUM      0x08   // Количество сокетов
#define SOCK_SIZE     2048   // Размер буфера сокета 
#define SOCK_BASE     0x1000 // Базовое смещение для блоков регистров сокетов
#define SOCK_REG_SIZE 0x0100 // Размер блока регистров сокета

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
  
#define SOCK_2_ADDR(n, offs) (SOCK_BASE + n * SOCK_REG_SIZE + offs) // Получает полный адрес по номеру сокета (n - номер сокета)


// ---- DHCP ----
#define DHCP_SERVER_PORT  67
#define DHCP_CLIENT_PORT  68
#define DHCP_TIMEOUT      6000 // (6 сек) Таймаут DHCP в мс
#define DHCP_RESP_TIMEOUT 4000 // (4 сек) Таймаут ожидания отсета в мс

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
 

// Первый байт в 16-битном слове будет старшим
#define htons(x) (((x & 0xFF00) >> 8) | (x & 0xFF) << 8)
// Первый байт в 16-битном слове будет младшим
#define ntohs(x) htons(x)

// Первый байт в 32-битном слове будет старшим
#define htonl(x) (((x & 0xFF000000) >> 24) | ((x & 0x00FF0000) >> 8) | ((x & 0x0000FF00) << 8) | ((x&0x000000FF) << 24))
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
 * IP 172.16.0.1 в uint32_t хранится как AС 10 00 01, т.е. 172 - младший октет и 
 * выводиться будет как  01 00 10 AC т.е. 172 - младший значащий байт
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

   // буферы передачи (от 0x8000 до 0xC00). размер буферов 2048
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


 // Прочесть регистр (от 1 до 4 байт)
 uint32_t _read_reg(const uint16_t addr, uint8_t bytes = 1) {
  uint32_t res = 0;
  bytes = constrain(bytes, 1, 4);
  _read(addr, (uint8_t*)&res, bytes);
  return res;
 }//_read_reg


 // Отправить данные по адресу 
 uint16_t _write(const uint16_t addr, uint8_t* buf, uint16_t size){
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
  
//  for(uint8_t i = 0; i < size; i++) Serial.println(buf[i], HEX);

  digitalWrite(CS_PIN, LOW); // Прижимаем CS 
  SPI.transfer16(addr); // Адрес отдаем старшим байтом вперед (transfer16 делает это сам)
  SPI.transfer(ctrl); // Байт управления
  SPI.transfer(buf, size) ; // Отправляем буфер  
  digitalWrite(CS_PIN, HIGH); // Отпускаем CS 
  return size; 
 }//_write


 // Записать значение (от 1 до 4 байт) в регистр 
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
 int8_t _sock_open(const uint8_t proto, const uint16_t port){
  SPI.beginTransaction(SPI_CONFIG);
  for(int8_t sock = 0; sock < SOCK_NUM; sock++){ // Ищем свободнй сокет
    if(_read_reg(SOCK_2_ADDR(sock, SOCK_STATUS_REG)) == SOCK_STAT_CLOSED){ // Читаем статус. Свободен ли
      // Свободен
      _write_reg(SOCK_2_ADDR(sock, SOCK_MODE_REG), proto); // В регистр режима пишем протокол
      _write_reg(SOCK_2_ADDR(sock, SOCK_INTERRUPT_REG), 0xFF); // В регистр прерываний пишем 0xFF
      _write_reg(SOCK_2_ADDR(sock, SOCK_SRC_PORT_REG), port, 2); // В регистр исходного порта - порт (2 байта)
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


 // Запись и чтение из сокета надо?
 

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

 // Разобрать ответ DHCP-сервера. Возвращает тип сообщения. Если таймаут: 255. Если все плохо: 0. Применяет полученные настройки
 uint8_t _dhcp_parse(){
  uint32_t t_start = millis();
  /*
  while(ждем пакета){ // Ждем ответа до таймаута
    if((millis() - t_start) > DHCP_RESP_TIMEOUT) return DHCP_MSG_TIMEOUT;
    delay(50);
  }//while
  */
  // Читаем DHCP заголоок
  // Проверяем id транзакции и всякие BOOTREPLY
  // получаем минимальные данные
  // Разгребаем опции
  /*
   * Скип - это примерно так
   *  opt_len = _dhcpUdpSocket.read();
   *  _dhcpUdpSocket.read((uint8_t *)NULL, opt_len);
   *
  */
  return 0;
 }//_dhcp_parse


 // Отправить запрос DHCP-серверу. Вернет 0, если все плохо
 // Принимает тип сообщения и параметры обмена в структура data)
 // Время, прошедшее с начала обмена считает сам 
 uint8_t _dhcp_send(const uint8_t msg_type, const DHCPData data){

  // -------------- Тут подумать!!!!!!!!!!!
  // для DISCOVER броадкаст на 255.255.255.255
  // для REQUEST нужен ip-сервера и наш локальный IP
    
  // Начинаем формировать UDP-пакет
  SPI.beginTransaction(SPI_CONFIG);
  _write_reg(SOCK_2_ADDR(data.sock, SOCK_DST_IP_REG), 0xFFFFFFFF, 4); // В регистр целевого IP - 255.255.255.255 (4 байта)
  _write_reg(SOCK_2_ADDR(data.sock, SOCK_DST_PORT_REG), DHCP_SERVER_PORT, 2); // В регистр целевого порта - порт (2 байта)


  // -------------- с записью разобраться!!!!!!!!!!! Там всякие смещения, маски и т.д.
  SPI.endTransaction();
  return 0;
 }//_dhcp_send

 
 // Получить (и установить) настройки через dhcp. Вернуть 0 если облом.
 // Результаты пишем в глобальные 
 // uint8_t mac[]   
 // uint32_t localIP
 // uint32_t netMask
 // uint32_t gwIP  
 // uint32_t dhcpIP
 
 uint8_t _dhcp_request(){
  uint8_t res = 0; 
  DHCPState state = START;
  DHCPData data; 

  // Открываем сокет
  data.sock = _sock_open(PROTO_UDP, DHCP_CLIENT_PORT);
  Serial.print("Socket: ");
  Serial.println(data.sock);

  if(data.sock < 0) return res; // Нет свободных сокетов

  // Формируем идентификатор транзакции
  data.trans_id = analogRead(P5); // Покатит?
  Serial.print("ID: ");
  Serial.println(data.trans_id);
  
  data.t_start = millis(); // Время начала запросов
  while((state != LEASED) && ((millis() - data.t_start) < DHCP_TIMEOUT)){ // Крутимся пока не получим настройки или не выйдет таймаут
    switch(state){
      case START: 
        //Отправляем запрос DISCOVER
        _dhcp_send(DHCP_MSG_DISCOVER, data);
        state = DISCOVER;
      break;

      case PREREQUEST: 
        //Отправляем запрос REQUEST
        state = REQUEST;
      break;

      case DISCOVER: 
        // Разбираем ответ
        // Если mesType OFFER, то опять отправляем запрос REQUEST
        state = REQUEST;
      break;

      case REQUEST: 
        // Разбираем ответ
        // Если mеsType ACK, то res = 1 и state = LEASED
        state = LEASED;
        // Если NAK, то state = START        
      break;
      
      default: break;
    }//switch

    // Если mesType = 255, то state = START и mesType = 0
  }//while

  // Закрываем сокет
  _sock_close(data.sock);
  return res;
 }//_dhcp_request
 
} //namespace


/*
* DHCP
* Генерируем ID транзакции
* Закрываем сокет
* Открываем на прослушивание udp сокет на клиентский порт
* В цикле пока не таймаут исходя из текущего состояния отправляем запросы, либо парсим ответы
* 
*/
/*
 * DHCP call stack: 
 * 1. beginWithDHCP | dhcp.cpp
 *   1. request_DHCP_lease | dhcp.cpp  
 *      1. udpSocket.stop | EthernetUDP.cpp
 *         1. socketClose | socket.cpp
 *            1. execCmd | *
 *      2. udpSocket.begin(client_port) | EthernetUDP.cpp      
 *         1. Ethernet.socketClose | socket.cpp
 *            1. execCmd | *
 *         2. Ethernet.socketBegin(UDP, client_port) | socket.cpp
 *            1. execCmd и всякое вокруг | 
 *      3. send_DHCP_MESSAGE | Dhcp.cpp     
 *         1. udpSocket.beginPacket(broadcast_addr, server_port) | EthernetUDP
 *            1. Ethernet.socketStartUDP(ip, port) | socket.cpp
 *               1. writeSnDIPR | ?
 *               2. writeDPORT  | ?
 *         2. Формирование пакета      
 *            1. udpSocket.write | EthernetUDP.cpp
 *               1. Ethernet.socketBufferData | socket.cpp
 *                  1. write_data | socket.cpp
 *                     1. write | *
 *         3. dhcpSocket.endPacket | EthernetUDP.cpp
 *            1. Ethernet.socketSendUDP | socket.cpp
 *               1. execCmd | *
 *               2. readSnIR | ?
 *               3. writeSnIR | ?
 *      4. parseDHCPRESPONSE | Dhcp.cpp
 *         1. udpSocket.parsePacket | EthernetUDP.cpp
 *            1. read | EthernetUDP.cpp
 *               1. Ethernet.socketRecv | socket.cpp
 *                  1. readSnSR | ?
 *                  2. writeSnRX_RD | ?
 *                  3. execCmd | *
 *            2. Ethernet.socketRecvAvailable | socket.cpp
 *               1. getSnRX_RSR | ?
 *            3. Ethernet.socketRecv | socket.cpp
 *                  1. readSnSR | ?
 *                  2. writeSnRX_RD | ?
 *                  3. execCmd | *
 *         2. udpSocket.read | EthernetUDP.cpp         
 *            1. Ethernet.socketRecv | socket.cpp
 *               1. readSnSR | ?
 *               2. writeSnRX_RD | ?
 *               3. execCmd | *
 *         3. udpSocket.remodePort() | EthernetClient.cpp  
 *            1. readSnDPORT(sockindex) | ?
 *         4. udpSocket.flush | EthernetUDP.cpp
 *         5. udpSocket.read | EthernetUDP.cpp         
 *            1. Ethernet.socketRecv | socket.cpp
 *               1. readSnSR | ?
 *               2. writeSnRX_RD | ?
 *               3. execCmd | *
 *         6. Ethernet.socketRecvAvailable | socket.cpp
 *            1. getSnRX_RSR | ?
 *         7. udpSocket.read | EthernetUDP.cpp         
 *         8. udpSocket.flush | EthernetUDP.cpp         
 *      5. udpSocket.stop | EthernetUDP.cpp
 *         1. socketClose | socket.cpp
 *            1. execCmd | *
 *      
 *      
*/
using namespace W5500Lite;


/*
  * Написать руками обмен по SPI, потом инициализацию и получение адреса по DHCP. 
 * Ну и может быть пингование гейтвея
*/


// == Main plugin function ==
void plgW5500Lite(){
  // Init
  SPI.begin();


  Serial.print("Init: ");
  Serial.println(_init());
  Serial.print("DHCP: ");
  Serial.println(_dhcp_request());


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
