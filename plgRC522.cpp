#include "plgRC522.h"
#include <SPI.h>

#define SDA_PIN  P0 //Green //SS
#define RST_PIN  P1 //Blue
#define MOSI_PIN P2 //Yellow
#define MISO_PIN P3 //Violet
#define SCK_PIN  P4 // Brown

#define READ_DELAY_MS 500 // 0.5 seconds between attempt

//#define SPI_CLOCK (4000000u) // 4MHz

// Registers
#define COMMAND_REG     0x01 // Command register 
#define T_MODE_REG      0x2A //
#define T_PRESCALER_REG 0x2B
#define T_RELOAD_REG_H  0x2C
#define T_RELOAD_REG_L  0x2D
#define TX_AUTO_REG     0x15
#define MODE_REG        0x11
#define TX_CONTROL_REG  0x14
#define COMM_I_EN_REG   0x02
#define COMM_IRQ_REG    0x04
#define FIFO_LEVEL_REG  0x0A
#define FIFO_DATA_REG   0x09
#define BIT_FRAMING_REG 0x0D
#define ERROR_REG       0x06
#define CONTROL_REG     0x0C
#define BIT_FRAMING_REG 0x0D
#define VERSION_REG     0x37


//Commands
#define CMD_RESET      0x0F // Reset command
#define CMD_TRANSCEIVE 0x0C // Send and receive command        
#define CMD_IDLE       0x00 // Отменить текущую команду
#define CMD_REQ_IDL    0x26 // Перейти в режим ожидания
#define CMD_ANTICOLL   0x93 // 

namespace RC522 { 

 void _write(const uint8_t reg, const uint8_t val){
//  SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE0));
  digitalWrite(SDA_PIN, LOW);
  SPI.transfer((reg << 1) & 0x7E); // ноль в начале и ноль в конце
  SPI.transfer(val);
  digitalWrite(SDA_PIN, HIGH);
//  SPI.endTransaction();
 }//_write


 uint8_t _read(const uint8_t reg){
  uint8_t res;
//  SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE0));
  digitalWrite(SDA_PIN, LOW);
  SPI.transfer(((reg << 1) & 0x7E) | 0x80); // старший бит ставим в 1
  res = SPI.transfer(0x00);
  digitalWrite(SDA_PIN, HIGH); 
//  SPI.endTransaction();
  return res;
 }//_read


 void _set_mask(const uint8_t reg, const uint8_t mask){
  uint8_t val = _read(reg);
  if(!(val & mask)) _write(reg, val | mask);
 }//_set_mask


 void _clear_mask(const uint8_t reg, const uint8_t mask){
  uint8_t val = _read(reg);
  _write(reg, val & (~mask));  
 }//_clear_mask

 
 void _init(){
  // Пины SS(SDA) и RST
  pinMode(SDA_PIN, OUTPUT);
  digitalWrite(SDA_PIN, HIGH); 
  
  pinMode(RST_PIN, OUTPUT);
  digitalWrite(RST_PIN, HIGH); // можно подергать для hard-reset-а

  // Hard reset
/*  
  pinMode(RST_PIN, INPUT);
  if(digitalRead(RST_PIN) == LOW){
    pinMode(RST_PIN, OUTPUT);
    digitalWrite(RST_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(RST_PIN, HIGH);
    delay(50);
  }//if
*/  
  // Soft-reset
  _write(COMMAND_REG, CMD_RESET);
  delay(50);
  
  // Сбрасываем скорости
  

  //Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
  _write(T_MODE_REG, 0x8D); //Tauto=1; f(Timer) = 6.78MHz/TPreScaler 
  _write(T_PRESCALER_REG, 0x3E); //TModeReg[3..0] + TPrescalerReg
  _write(T_RELOAD_REG_L, 0x1E);
  _write(T_RELOAD_REG_H, 0x00);
  _write(TX_AUTO_REG, 0x40); // Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
  _write(MODE_REG, 0x3D); //Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)

  // Antenna On
  _set_mask(TX_CONTROL_REG, 0x03);
 }//_init


 uint8_t _transfer(uint8_t *sndBuf, const uint8_t sndSize, uint8_t *rcvBuf, uint8_t rcvSize, uint8_t &rcvBits){
  _write(COMM_I_EN_REG, 0xF7); // Разрешаем прерывания 0x77 | 0x80
  _clear_mask(COMM_IRQ_REG, 0x80); // Сбрасываем биты запроса прерывания
  _set_mask(FIFO_LEVEL_REG, 0x80); // Очищаем и инициализируем fifo
  _write(COMMAND_REG, CMD_IDLE); // Отменяем текущую выполняемую команду

  // Отправляем данные
  for(uint8_t i = 0; i < sndSize; i++) _write(FIFO_DATA_REG, sndBuf[i]);

  _write(COMMAND_REG, CMD_TRANSCEIVE);// Даем команду на получение данных
  _set_mask(BIT_FRAMING_REG, 0x80);//StartSend=1,transmission of data starts

  //Ждем завершения получения данных
  // счетчик по тактовой частоте. Максимальное время ожидания 25мс
  int i = 2000;
  for(; i > 0; i--){
    // ComIrqReg[7..0]: Set1 TxIRq RxIRq[x] IdleIRq[x] HiAlertIRq LoAlertIRq ErrIRq TimerIRq[x]
    uint8_t res = _read(COMM_IRQ_REG);
    if(res & 0x01) return 0; // Таймаут
    if(res & 0x30) break; // Все прошло успешно
    yield();
  }//for
  if(!i) return 0; // Таймаут
  _clear_mask(BIT_FRAMING_REG, 0x80); // StartSend=0

  // Теперь проверяем, что б небыло коллизий и ошибок
  //ErrorReg[7..0]: WrErr TempErr reserved BufferOvfl[x] CollErr[x] CRCErr ParityErr[x] ProtocolErr[x] = 0x1B
  if(_read(ERROR_REG) & 0x1B) return 0; // Были ошибки передачи
  
  //?? NOTAGERR

  // Читаем данные, если есть
  uint8_t avail = _read(FIFO_LEVEL_REG);// Сколько байт данных нас ждут
  uint8_t last_bits = _read(CONTROL_REG) & 0x07; //RxLastBits[2:0]. Сколько значащих бит будет в последнем байте. Если = 0, то весь байт значащий.
  rcvBits = avail << 3; // по уполчанию (last_bits == 0) в ответе avail * 8 бит
  if(last_bits) rcvBits = ((avail - 1) << 3) + last_bits; // Сколько всего бит в ответе  
    
  rcvSize = min(avail, rcvSize); // Читаем сколько ждут, но не более, чем размер буфера

  for(i = 0; i < rcvSize; i++) rcvBuf[i] = _read(FIFO_DATA_REG);
  
  return 1; // все OK
 }//_transfer


 uint8_t _is_card_ready(){
  uint8_t rcv_bits;
  uint8_t buf[2];
  uint8_t res;

  buf[0] = CMD_REQ_IDL;
  _write(BIT_FRAMING_REG, 0x07);
  res = _transfer(buf, 1, buf, 2, rcv_bits);  
  if(!res) return res; // Вообще ничего не считалось (карты нету)
/*
 * В буфер вернут тип карты:
 * 0x4400 = Mifare_UltraLight
 * 0x0400 = Mifare_One(S50)
 * 0x0200 = Mifare_One(S70)
 * 0x0800 = Mifare_Pro(X)
 * 0x4403 = Mifare_DESFire
*/
/*
  Serial.print("bits: ");
  Serial.println(rcv_bits, HEX);
  Serial.print("Type: 0x");
  Serial.print(buf[1], HEX);
  Serial.println(buf[0], HEX);
//  Serial.println(res);
*/
//  Serial.println(_read(VERSION_REG), HEX);
  return rcv_bits == 0x10; // Если количеств значащих бит в ответе != 0x10 то тоже все плохо
 }//_is_card_ready


// Переделать на получение серийника в массив!!!!!!!!!!!!!1
 uint32_t _get_uid(){
  _write(BIT_FRAMING_REG, 0x00); //TxLastBists = BitFramingReg[2..0]
  uint8_t buf[] = {CMD_ANTICOLL, 0x20, 0, 0, 0};
  uint8_t rvc_bits;
  uint8_t res = _transfer(buf, 2, buf, sizeof(buf), rvc_bits);
  if(!res) return 0;

  uint8_t crc = 0;
  for(uint8_t i = 0; i < 4 ; i++) crc ^= buf[i]; //Праямо тут и складывать в выходной массив
  if(crc != buf[4]) return 0; // crc error

  return ((uint32_t)buf[0] << 24)| ((uint32_t)buf[1] << 16) | ((uint32_t)buf[2] << 8) | ((uint32_t)buf[3]);     
 }//_get_uid
} //namespace

/*
 * https://github.com/song940/RFID-RC522/blob/master/RFID.cpp
 * 
 * https://github.com/miguelbalboa/rfid/blob/master/src/MFRC522.cpp
 * 
* 
* 
*/
using namespace RC522;
// == Main plugin function ==
void plgRC522(){
  // Init
  SPI.begin();
  _init();

  // Дописать интерфейс(по аналогии с RDM6300)
  
  // Main loop
  while(1){
    if(_is_card_ready()) {
      //uint8_t uid[4] = _get_uid(uid);
      Serial.println(_get_uid(), HEX);
      
      }
    delay(READ_DELAY_MS);    
  }//while  
}//plgRC522
