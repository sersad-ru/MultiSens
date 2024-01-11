#include "plgADXL345.h"
#include <Wire.h>


#define SDA_PIN P6 //Yellow-Black
#define SCL_PIN P7 //Gray-Black
#define READ_DELAY_MS 500 // 0.5 seconds between attempt

#define OFFSET_DEFAULT 0 // Default offset for all axis

#define ADXL_ADDRESS 0x53  // ADXL I2C address
#define POWER_CTL 0x2D //Power register
#define POWER_CTL_VALUE 0x08 // Set D3 bit to 1 to start measure

#define DATA_FORMAT 0x31 // Data format register
#define DATA_FORMAT_VALUE 0x0b // 16g range

#define OFSX 0x1e // X-offset register
#define OFSY 0x1f // Y-offset register
#define OFSZ 0x20 // Z-offset register

#define DATA_REG_START 0x32 // First data register
#define DATA_REG_SIZE 0x06  // Size of data register block

#define MODE_NORM 0 // Display current values
#define MODE_MIN -1 // Display minimal valuse
#define MODE_MAX +1 // Display maximal values

namespace ADXL345 {
typedef struct {
  int16_t x;
  int16_t y;
  int16_t z;
} accPkt;  

typedef struct {
  int16_t minX;
  int16_t minY;
  int16_t minZ;
  int16_t maxX;
  int16_t maxY;
  int16_t maxZ;  
} accMinMax;

accPkt _read_acc(){
  accPkt res;
  core.i2cRequestRead(ADXL_ADDRESS, DATA_REG_START, DATA_REG_SIZE);
  res.x = Wire.read() | Wire.read() << 8; //LSB | HSB
  res.y = Wire.read() | Wire.read() << 8; //LSB | HSB
  res.z = Wire.read() | Wire.read() << 8; //LSB | HSB  
  return res;
}//_read_acc


accMinMax _set_minmax(const accPkt pkt){
  accMinMax res;
  res.minX = pkt.x;
  res.maxX = pkt.x;
  res.minY = pkt.y;
  res.maxY = pkt.y;
  res.minZ = pkt.z;
  res.maxZ = pkt.z;
  return res;
}//_set_minmax


accMinMax _calc_minmax(accMinMax minmax, const accPkt pkt){
  minmax.minX = min(pkt.x, minmax.minX);
  minmax.minY = min(pkt.y, minmax.minY);
  minmax.minZ = min(pkt.z, minmax.minZ);
  minmax.maxX = max(pkt.x, minmax.maxX);
  minmax.maxY = max(pkt.y, minmax.maxY);
  minmax.maxZ = max(pkt.z, minmax.maxZ);  
  return minmax;
}//_calc_minmax


void _print_val(const char name, const char sym, const int16_t val){
  core.print(name);
  core.print(sym);
  core.printValScale(core, (val + 5) / 10);
  core.print(" ");
  core.printValScale(Serial, val, 100);
  Serial.print(" ");
}//_print_val


int8_t _calibrate(const char axis){
  core.moveCursor(0, 1);
  core.print(axis);
  core.print(MS_SYM_ARROW_UP_CODE);
  core.print(" ");
  core.print(MS_SYM_SELECT_CODE);
  core.println(F("-start"));
  while(core.wait4Button() != SELECT);

  core.moveCursor(0, 1);
  uint32_t value = 0;
  for(uint8_t i = 0; i < 16; i++)
  {
    accPkt acc = _read_acc(); 
    switch(axis){
      case 'X': value += acc.x; break;
      case 'Y': value += acc.y; break;
      case 'Z': value += acc.z; break;
      default: break;
    }//switch 
    core.print(MS_SYM_PROGRESS_CODE);
    delay(100);
  }//for
  
  return (256 - (value >> 4)) >> 2; // middle = 16 reads / 16(>>4). 256 - middle / 4(>>2)
}//_calibrate
} //namespace


using namespace ADXL345;

// == Main plugin function ==
void plgADXL345(){
  // Init
  // Load settings from EEPROM 
  if(!core.loadSettings((uint8_t*)&plgADXL345Cfg)){
    plgADXL345Cfg.offX = OFFSET_DEFAULT;// Settings was reseted. Use default values
    plgADXL345Cfg.offY = OFFSET_DEFAULT;
    plgADXL345Cfg.offZ = OFFSET_DEFAULT;
    core.saveSettings((uint8_t*)&plgADXL345Cfg);// Save default value  
  }//if  

  // Init device
  Wire.begin();
  core.i2cWriteReg(ADXL_ADDRESS, POWER_CTL, POWER_CTL_VALUE);  
  core.i2cWriteReg(ADXL_ADDRESS, DATA_FORMAT, DATA_FORMAT_VALUE);  
  core.i2cWriteReg(ADXL_ADDRESS, OFSX, plgADXL345Cfg.offX);  
  core.i2cWriteReg(ADXL_ADDRESS, OFSY, plgADXL345Cfg.offY);  
  core.i2cWriteReg(ADXL_ADDRESS, OFSZ, plgADXL345Cfg.offZ);  
  
  accMinMax minmax = _set_minmax(_read_acc());// Read first values

  Serial.print(F("X(g), Y(g), Z(g), ("));
  Serial.print(READ_DELAY_MS);
  Serial.println(FF(MS_MSG_DELAY_END));

  int8_t mode = MODE_NORM;  
  // Main loop
  while(1){
    // Process user input    
    switch (core.getButton()) {
      case UP:
      case UP_LONG: mode = (mode == MODE_MAX) ? MODE_NORM : MODE_MAX; break;
      
      case DOWN:
      case DOWN_LONG: mode = (mode == MODE_MIN) ? MODE_NORM : MODE_MIN; break;

      case SELECT: 
        plgADXL345Cfg.offX = _calibrate('X');
        plgADXL345Cfg.offY = _calibrate('Y');
        plgADXL345Cfg.offZ = _calibrate('Z');       
        core.i2cWriteReg(ADXL_ADDRESS, OFSX, plgADXL345Cfg.offX);  
        core.i2cWriteReg(ADXL_ADDRESS, OFSY, plgADXL345Cfg.offY);  
        core.i2cWriteReg(ADXL_ADDRESS, OFSZ, plgADXL345Cfg.offZ);  

      break;  
      case SELECT_LONG: core.saveSettings((uint8_t*)&plgADXL345Cfg);break;   // save settings to EEPROM
      
      default: break;
    }//switch
   
    accPkt acc = _read_acc(); // Read values
    minmax = _calc_minmax(minmax, acc);
    core.moveCursor(0, 1); // First symbol of second line

    switch(mode) {
      case MODE_NORM:
        _print_val('X', ':', (acc.x * 100) >> 8);
        _print_val('Y', ':', (acc.y * 100) >> 8);
        _print_val('Z', ':', (acc.z * 100) >> 8);
      break;
      case MODE_MAX:
        _print_val('X', MS_SYM_ARROW_UP_CODE, (minmax.maxX * 100) >> 8);
        _print_val('Y', MS_SYM_ARROW_UP_CODE, (minmax.maxY * 100) >> 8);
        _print_val('Z', MS_SYM_ARROW_UP_CODE, (minmax.maxZ * 100) >> 8);      
      break;
      case MODE_MIN:
        _print_val('X', MS_SYM_ARROW_DOWN_CODE, (minmax.minX * 100) >> 8);
        _print_val('Y', MS_SYM_ARROW_DOWN_CODE, (minmax.minY * 100) >> 8);
        _print_val('Z', MS_SYM_ARROW_DOWN_CODE, (minmax.minZ * 100) >> 8);      
      break;     
      default: break;
    }//switch
    core.println();
    Serial.println();
    delay(READ_DELAY_MS);
  }//while  
}//plgADXL345
