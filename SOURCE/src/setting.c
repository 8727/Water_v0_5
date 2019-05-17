#include "setting.h"

struct settingsInitTypeDef settings;
static __IO uint32_t msTicks;

void SysTick_Handler(void){
  msTicks++;
}

uint32_t GetTick(void){
  return msTicks;
}

void DelayMs(uint32_t ms){
  uint32_t tickstart = GetTick();
  while((GetTick() - tickstart) < ms){}
}

void Start(void){
  SysTick_Config(SystemCoreClock / 1000);   //1ms
  
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  
  AFIO->MAPR = AFIO_MAPR_SWJ_CFG_JTAGDISABLE + AFIO_MAPR_TIM2_REMAP_FULLREMAP + AFIO_MAPR_TIM4_REMAP + AFIO_MAPR_I2C1_REMAP;
  
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPGEN;
}

void CounterToBuffer(uint32_t counter, uint8_t* buff){
  buff[0x00] = (counter >> 0x18);
  buff[0x01] = (counter >> 0x10);
  buff[0x02] = (counter >> 0x08);
  buff[0x03] = counter; 
}

uint32_t BufferToCounter(uint8_t* buff){
  uint32_t data = buff[0x00] << 0x18;
  data |= buff[0x01] << 0x10;
  data |= buff[0x02] << 0x08;
  data |= buff[0x03]; 
  return data;
}

void ReadSettingTimers(void){
  uint8_t tempBuff[0x80];
  uint8_t step = 0x00;
  Ee24cxxRead(0x80, tempBuff, 0x7F);
  for(uint8_t i = 0x00; i < 0x10; i++){
    timesJob[i].jobActivity = tempBuff[step];
    timesJob[i].typeChannel = tempBuff[step + 0x01];
    timesJob[i].value = tempBuff[step + 0x02];
    timesJob[i].wday = tempBuff[step + 0x03];
    timesJob[i].hourOn = tempBuff[step + 0x04];
    timesJob[i].minOn = tempBuff[step + 0x05];
    timesJob[i].hourOff = tempBuff[step + 0x06];
    timesJob[i].minOff = tempBuff[step + 0x07];
    step += 0x08;
  }
}
void ReadConfig(void){
  uint8_t tempReadBuff[0x30];
  uint8_t tempWriteBuff[0x08];
  Ee24cxxRead(0x00, tempReadBuff, 0x30);
  if(0xFF != tempReadBuff[EEPROM_STATUS]){
    RtcTypeDef unixTime;
    unixTime.year  = BUILD_YEAR;
    unixTime.month = BUILD_MONTH;
    unixTime.day   = BUILD_DAY;
    unixTime.hour  = BUILD_TIME_H;
    unixTime.min   = BUILD_TIME_M;
    unixTime.sec   = BUILD_TIME_S;
    CounterToBuffer(RtcTimeToCounter(&unixTime), tempWriteBuff);
    Ee24cxxWritePage(EEPROM_BUILD_DATE, tempWriteBuff, 0x04);
    Ee24cxxWriteByte(EEPROM_STATUS, 0x00);
    Ee24cxxWriteByte(EEPROM_DEVICE_N, DEVICE_NUMBER);
    Ee24cxxWriteByte(EEPROM_BUILD_TYPE,'S');
    Ee24cxxWriteByte(EEPROM_CALIBRATION, RTC_CALIBRATION);
    Ee24cxxWritePage(EEPROM_NAME_BUILD, (uint8_t*)NAME_BUILD, 0x05);
    Ee24cxxWriteByte(EEPROM_CALIB_POWER_V, CALIB_POWER_V);
    CounterToBuffer(CAN_SPEED, tempWriteBuff);
    Ee24cxxWritePage(EEPROM_CAN_SPEED, tempWriteBuff, 0x08);
    Ee24cxxWriteByte(EEPROM_RS485_SPEED, ((uint8_t)RS485_SPEED >> 0x08));
    Ee24cxxWriteByte(EEPROM_RS485_SPEED + 0x01, (uint8_t)RS485_SPEED);
    
    
//    for(uint8_t i = 0x80; i < 0xFF; i += 0x08){
//      Ee24cxxWritePage(i, 0x00, 0x08);
//    }
    settings.rotation = 0x09;
    
    Ee24cxxRead(0x00, tempReadBuff, 0x30);
  }
  settings.type = tempReadBuff[EEPROM_BUILD_TYPE];
  settings.number = tempReadBuff[EEPROM_DEVICE_N];
  settings.date = BufferToCounter(tempReadBuff);
  settings.canDevice = 0x0000 + (settings.number << 0x04);
  settings.calibration = tempReadBuff[EEPROM_CALIBRATION];
  settings.calibPowerV = tempReadBuff[EEPROM_CALIB_POWER_V];
  settings.canSpeed = (tempReadBuff[EEPROM_CAN_SPEED] << 0x18)| (tempReadBuff[EEPROM_CAN_SPEED + 0x01] << 0x10) |
                      (tempReadBuff[EEPROM_CAN_SPEED + 0x02] << 0x08)| (tempReadBuff[EEPROM_CAN_SPEED + 0x03]);
//  settings.rs485Speed = (tempReadBuff[EEPROM_RS485_SPEED] << 0x08)| tempReadBuff[EEPROM_RS485_SPEED + 0x01];
  settings.rs485Speed = RS485_SPEED;
  
  
  
  
  settings.rotation = 0x09;
  switch(settings.rotation){
    case 0x27:  //Dspl_Rotation_270
    case 0x09:  //Dspl_Rotation_90
      settings.maxX = 0x01E0; 
      settings.maxY = 0x0140;
    break; 
    default:  //Dspl_Rotation_0 Rotation_180
      settings.maxX = 0x0140; 
      settings.maxY = 0x01E0;
    break;
  }
  settings.ax = 0x07;
  settings.ay = 0x0A;
  settings.bx = 0x0005;
  settings.by = 0xFFBD;
}

void Setting(void){
  Ee24cxxInit();
  ReadConfig();
  RtcInit();
  LcdInit();
  W25QxxInit();
  GuiInit();
  Xpt2046Init();
  if(!(GPIOB->IDR & GPIO_IDR_IDR11)){
    Xpt2046Calibration();
  }else{
    TIM2->CCR1 = 0x2F;
  }
  Rs485Init();
  Dht22Init();
  Ds18b20Init();
  
  Ws2811Init();
  
  ReadSettingTimers();
  
//  TIM2->CCR1 = 0x20;
//  DelayMs(100);
//  W25QxxEraseBlocks();
}
