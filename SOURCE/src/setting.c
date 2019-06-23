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

void ReadConfig(void){
  uint8_t tempReadBuff[0x30];
//  uint8_t tempWriteBuff[0x08];
  Ee24cxxRead(0x00, tempReadBuff, 0x30);
//  if(0xFF != tempReadBuff[ADDR_EEPROM_STATUS]){
//    RtcTypeDef unixTime;
//    unixTime.year  = BUILD_YEAR;
//    unixTime.month = BUILD_MONTH;
//    unixTime.day   = BUILD_DAY;
//    unixTime.hour  = BUILD_TIME_H;
//    unixTime.min   = BUILD_TIME_M;
//    unixTime.sec   = BUILD_TIME_S;
//    CounterToBuffer(RtcTimeToCounter(&unixTime), tempWriteBuff);
//    Ee24cxxWritePage(ADDR_EEPROM_BUILD_DATE, tempWriteBuff, 0x04);
//    Ee24cxxWriteByte(ADDR_EEPROM_STATUS, 0x00);
//    Ee24cxxWriteByte(ADDR_EEPROM_DEVICE_N, DEVICE_NUMBER);
//    Ee24cxxWriteByte(EEPROM_BUILD_TYPE,'S');
//    Ee24cxxWriteByte(EEPROM_CALIBRATION, RTC_CALIBRATION);
//    Ee24cxxWritePage(EEPROM_NAME_BUILD, (uint8_t*)NAME_BUILD, 0x05);
//    Ee24cxxWriteByte(EEPROM_CALIB_POWER_V, CALIB_POWER_V);
//    CounterToBuffer(CAN_SPEED, tempWriteBuff);
//    Ee24cxxWritePage(EEPROM_CAN_SPEED, tempWriteBuff, 0x08);
//    Ee24cxxWriteByte(EEPROM_RS485_SPEED, ((uint8_t)RS485_SPEED >> 0x08));
//    Ee24cxxWriteByte(EEPROM_RS485_SPEED + 0x01, (uint8_t)RS485_SPEED);
//    
//    
//    for(uint8_t i = 0x80; i < 0xFF; i += 0x08){
//      Ee24cxxWritePage(i, 0x00, 0x08);
//    }
    settings.rotation = 0x09;
    
    Ee24cxxRead(0x00, tempReadBuff, 0x30);
//  }
//  settings.type = tempReadBuff[ADDR_EEPROM_BUILD_TYPE];
//  settings.number = tempReadBuff[ADDR_EEPROM_DEVICE_N];
  settings.dateBuild = BufferToCounter(tempReadBuff);
  settings.canDevice = 0x0000 + (settings.number << 0x04);
//  settings.rtcCalibration = tempReadBuff[ADDR_EEPROM_CALIBRATION];
//  settings.calibPowerV = tempReadBuff[ADDR_EEPROM_CALIB_POWER_V];
  settings.canSpeed = (tempReadBuff[ADDR_EEPROM_CAN_SPEED] << 0x18)| (tempReadBuff[ADDR_EEPROM_CAN_SPEED + 0x01] << 0x10) |
                      (tempReadBuff[ADDR_EEPROM_CAN_SPEED + 0x02] << 0x08)| (tempReadBuff[ADDR_EEPROM_CAN_SPEED + 0x03]);
//  settings.rs485Speed = (tempReadBuff[EEPROM_RS485_SPEED] << 0x08)| tempReadBuff[EEPROM_RS485_SPEED + 0x01];
  settings.rs485Speed = RS485_SPEED;
  
  

  switch(settings.rotation){
    case 0x27:  //Dspl_Rotation_270
    case 0x09:  //Dspl_Rotation_90
      settings.maxX = 0x01E0; 
      settings.maxY = 0x0140;
    break; 
    default:    //Dspl_Rotation_0 Rotation_180
      settings.maxX = 0x0140; 
      settings.maxY = 0x01E0;
    break;
  }
  
  heating.valveDefDelay = 100;   // 10sec * 10Hz
  heating.valveInterval = 500;   // 50sec * 10Hz
  heating.interval = 600;        // 60sec * 10Hz
  heating.gistTemperature = 3;   // 0.3 * 10
  heating.stepDelay = 2;         // 0.2sec * 10Hz
  heating.topTemperature = 500;  // 50 * 10
  heating.maxDelay = 200;        // 20sec * 10Hz
  
  fan.interval = 9000;           // 15min * 60sec * 10Hz
  fan.delay = 6000;              // 10min * 60sec * 10Hz
  fan.gistHumidity = 30;         // 3 * 10
  fan.gistTemperature = 0x00;    // 
  fan.maxHumidity = 800;         // 80 * 10
  fan.maxTemperature = 0x00;     // 
  fan.sensorInterval = 600;       // 60sec * 10Hz
}

void TIM6_IRQHandler(void){
  TIM6->SR &= ~TIM_SR_UIF;
  HeatingPWM();
  FanAnalyze();
  ADCAlarm();
}

void Timer10Hz(void){  
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
  TIM6->PSC = 0x1F3F; // 7999 80000000:8000=10000Hz
  TIM6->ARR = 0x03E7; // 10Hz
  TIM6->SR = 0x00;
  TIM6->DIER |= TIM_DIER_UIE;
  TIM6->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE;
  
  NVIC_SetPriority(TIM6_IRQn, PRIORITY_HEATING);
  NVIC_EnableIRQ(TIM6_IRQn);
}

void Setting(void){
  SysTick_Config(SystemCoreClock / 1000);   //1ms
  
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;
  RCC->APB1ENR |= RCC_APB1ENR_BKPEN;
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  
//  AFIO->MAPR = AFIO_MAPR_SWJ_CFG_JTAGDISABLE + AFIO_MAPR_TIM2_REMAP_FULLREMAP + AFIO_MAPR_TIM4_REMAP + AFIO_MAPR_I2C1_REMAP;
  AFIO->MAPR = AFIO_MAPR_SWJ_CFG_JTAGDISABLE + AFIO_MAPR_TIM2_REMAP_FULLREMAP + AFIO_MAPR_TIM4_REMAP;
  
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPFEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPGEN;
  
  uint8_t info = 0x00;
  info = InInit(); 
  #if defined(INFO)
    if(0x00 == info){ printf("<ERROR>"); }else{ printf("< OK >"); }
    printf("    Initialization PORTS\r\n");
  #endif
  ADCInit();
  #if defined(INFO)
    printf("< OK >    Initialization SENSORS\r\n");
  #endif
  GidrolockInit();
  #if defined(INFO)
    printf("< OK >    Initialization CRANES\r\n");
  #endif
  HeatingInit();
  #if defined(INFO)
    printf("< OK >    Initialization HEATING\r\n");
  #endif
  FanInit();
  #if defined(INFO)
    printf("< OK >    Initialization FAN\r\n");
  #endif
  Ee24cxxInit();
  #if defined(INFO)
    printf("< OK >    Initialization EE24Cxx\r\n");
  #endif
  ReadConfig();
  #if defined(INFO)
    printf("< OK >    Read configuration\r\n");
  #endif
  Timer10Hz();
  #if defined(INFO)
    printf("< OK >    Start Timer 10Hz\r\n");
  #endif
  RtcInit();
  #if defined(INFO)
    printf("< OK >    Initialization RTC\r\n");
  #endif
  Ds18b20Init();
  #if defined(INFO)
    printf("< OK >    Initialization DS18B20\r\n");
  #endif
  Dht22Init();
  #if defined(INFO)
    printf("< OK >    Initialization DHT22\r\n");
  #endif
  Rs485Init();
  #if defined(INFO)
    printf("< OK >    Initialization RS485\r\n");
  #endif
  W25QxxInit();
  #if defined(INFO)
    printf("< OK >    Initialization W25Qxx\r\n");
  #endif
  LcdInit();
  #if defined(INFO)
    printf("< OK >    Initialization LCD\r\n");
  #endif
  GuiInit();
  #if defined(INFO)
    printf("< OK >    Initialization GUI\r\n");
  #endif
  BeepInit();
  #if defined(INFO)
    printf("< OK >    Initialization BEEP\r\n");
  #endif
  
//  W25QxxEraseBlocks();
}
