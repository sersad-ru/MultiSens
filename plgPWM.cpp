#include "plgPWM.h"


#define CH1_PIN P1
#define CH2_PIN P2
#define CH3_PIN P3

#define CH1_NUM 0
#define CH2_NUM 1
#define CH3_NUM 2

#define DEFAULT_DUTY 0

#define PWM_MASK_REG TIMSK2 // Регист маски прерывания таймера для опроса кнопки (TIMER1, канал A, штатный делитеть /64)
#define PWM_MASK_BIT TOIE1  // Бит маски перерывания таймера для опроса кнопки (по переполнению 490 раз в секунду)

// Манипуляции с портами
#define _PORT_OUTPUT(P, M)   (P |=  (M))           // Сконфигурировать порт на вывод
#define _PORT_INPUT(P, M)    (P &= ~(M))           // Сконфигурировать порт на ввод
#define _PORT_SET_LOW(P, M)  (P &= ~(M))           // Сбросить пин
#define _PORT_SET_HIGH(P, M) (P |=  (M))           // Установить пин
#define _PORT_READ(P, M)    ((P &   (M)) ? 1 : 0)  // Прочесть пин

// Макросы и переменная для сохранения статусного регистра и запрета/разрешения прерываний
#define _INT_OFF _PWM_oldSREG = SREG; cli()
#define _INT_ON SREG = _PWM_oldSREG
uint8_t _PWM_oldSREG;


void _set_pwm(uint8_t pin, uint8_t duty){
/*  
  if(pin == CH3_PIN){ 
    pinMode(CH3_PIN, OUTPUT);
    digitalWrite(CH3_PIN, HIGH);
    return;
  }
*/  
  analogWrite(pin, duty);
  Serial.print("TCCR2A: 0x");
  Serial.println(TCCR2A, HEX);
  Serial.print("TCCR2B: 0x");
  Serial.println(TCCR2B, HEX);
  Serial.print("TIMSK2: 0x");
  Serial.println(TIMSK2, HEX);
  
}//_set_pwm


// == Main plugin function ==
void plgPWM(MultiSensCore& core){
  // Init
  pinMode(CH3_PIN, OUTPUT);
  
  // Load settings from EEPROM 
  if(!core.loadSettings((uint8_t*)&plgPWMCfg)){
    plgPWMCfg.duty[CH1_NUM] = DEFAULT_DUTY;// Settings was reseted. Use default values
    plgPWMCfg.duty[CH2_NUM] = DEFAULT_DUTY;// Settings was reseted. Use default values
    plgPWMCfg.duty[CH3_NUM] = DEFAULT_DUTY;// Settings was reseted. Use default values    
    core.saveSettings((uint8_t*)&plgPWMCfg);// Save default value  
  }//if  

  // Прерывания таймера для опроса клавиатуры
  _INT_OFF;
  _PORT_SET_HIGH(PWM_MASK_REG, _BV(PWM_MASK_BIT));  // По переполнению. По сравнению одновременно с PWM не работает. В результате 490 раз в секунду. 
  _INT_ON;

  
  // Display Init
  core.moveCursor(0, 1); // First symbol of second line
  core.print(core.getPinName(CH1_PIN));  
  core.moveCursor(7, 1); 
  core.print(core.getPinName(CH2_PIN));
  core.moveCursor(14, 1); 
  core.print(core.getPinName(CH3_PIN));
  core.setCursorType(BLOCK);

  //  
  const uint8_t offsets[] = {3, 10, 17};
  const uint8_t pins[] = {CH1_PIN, CH2_PIN, CH3_PIN};  
  uint8_t cur_channel = 0;

  // Start with current duties
  for(uint8_t i = 0; i < 3; i++){
    core.moveCursor(offsets[i], 1);
    core.print(core.rAlign(plgPWMCfg.duty[i], 3));
    Serial.print(core.getPinName(pins[i]));
    Serial.println(plgPWMCfg.duty[i]);
    _set_pwm(pins[i], plgPWMCfg.duty[i]); // Start pwm
  }//for
  core.moveCursor(offsets[cur_channel] - 3, 1);
    
  // Main loop
  while(1){
    // Process user input    
    switch (core.getButton()) {
      case UP:
      case UP_LONG:  
        plgPWMCfg.duty[cur_channel] ++;
        core.moveCursor(offsets[cur_channel], 1);
        core.print(core.rAlign(plgPWMCfg.duty[cur_channel], 3));
        core.moveCursor(offsets[cur_channel] - 3, 1);
        Serial.print(core.getPinName(pins[cur_channel]));
        Serial.println(plgPWMCfg.duty[cur_channel]);        
        _set_pwm(pins[cur_channel], plgPWMCfg.duty[cur_channel]); // Change pwm
      break;
      
      case DOWN:
      case DOWN_LONG:  
        plgPWMCfg.duty[cur_channel] --;
        core.moveCursor(offsets[cur_channel], 1);
        core.print(core.rAlign(plgPWMCfg.duty[cur_channel], 3));
        core.moveCursor(offsets[cur_channel] - 3, 1);
        Serial.print(core.getPinName(pins[cur_channel]));
        Serial.println(plgPWMCfg.duty[cur_channel]);        
        _set_pwm(pins[cur_channel], plgPWMCfg.duty[cur_channel]); // Change pwm
      break;
      
      case SELECT: 
        cur_channel++;
        if(cur_channel > 2) cur_channel = 0;
        core.moveCursor(offsets[cur_channel] - 3, 1);
      break;  
      
      case SELECT_LONG: core.saveSettings((uint8_t*)&plgPWMCfg); break;   // save settings to EEPROM
      
      default: break;
    }//switch    
  }//while
}//plgPWM

volatile uint8_t _duty3 = 0;
// Обработчик прерывания для програмного PWM
// По пререполнению. По сравнению одновременно с PWM не работает. В результате 490 раз в секунду.
ISR(TIMER2_OVF_vect){
  static volatile uint8_t cnt = 0;
  if(!cnt) digitalWrite(CH3_PIN, HIGH);
  if(cnt == _duty3) digitalWrite(CH3_PIN, LOW);
  cnt++;
}//ISR
