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

void WriteData32ToBuffer(uint8_t addr, uint32_t data, uint8_t* buff){
  buff[addr] = (data >> 0x18);
  buff[addr + 0x01] = (data >> 0x10);
  buff[addr + 0x02] = (data >> 0x08);
  buff[addr + 0x03] = data; 
}

void WriteData16ToBuffer(uint8_t addr, uint16_t data, uint8_t* buff){
  buff[addr] = (data >> 0x08);
  buff[addr + 0x01] = data;
}

uint32_t ReadData32Buffer(uint8_t addr, uint8_t* buff){
  uint32_t data = buff[addr] << 0x18;
  data |= buff[addr + 0x01] << 0x10;
  data |= buff[addr + 0x02] << 0x08;
  data |= buff[addr + 0x03]; 
  return data;
}

uint16_t ReadData16Buffer(uint8_t addr, uint8_t* buff){
  uint16_t data = buff[addr] << 0x08;
  data |= buff[addr + 0x01];
  return data;
}

void ReadConfig(void){
  uint8_t buffEeprom[EEPROM_BUFF];
  Ee24cxxRead(buffEeprom);
  if(0xFF != buffEeprom[ADDR_STATUS]){
    RtcTypeDef dateBuild;
    dateBuild.year  = BUILD_YEAR;
    dateBuild.month = BUILD_MONTH;
    dateBuild.day   = BUILD_DAY;
    dateBuild.hour  = BUILD_TIME_H;
    dateBuild.min   = BUILD_TIME_M;
    dateBuild.sec   = BUILD_TIME_S;
    WriteData32ToBuffer(ADDR_DATE_BUILD, RtcTimeToSeconds(&dateBuild), buffEeprom);
    buffEeprom[ADDR_STATUS] = STATUS;
    buffEeprom[ADDR_DEVICE_NUMBER] = DEVICE_NUMBER;
    buffEeprom[ADDR_DEVICE_TYPE] = DEVICE_TYPE;
    buffEeprom[ADDR_RTC_CALIBRATION] = RTC_CALIBRATION;
    WriteData32ToBuffer(ADDR_CAN_SPEED, CAN_SPEED, buffEeprom);
    WriteData16ToBuffer(ADDR_RS485_SPEED, RS485_SPEED, buffEeprom);
    
    buffEeprom[ADDR_LCD_ROTATION] = LCD_ROTATION;
    buffEeprom[ADDR_TIME_ZONE] = TIME_ZONE;
    //RF24L01
    buffEeprom[ADDR_RF24_SP_PW] = RF24_SP_PW;
    buffEeprom[ADDR_RF24_CH] = RF24_CH;
    WriteData32ToBuffer(ADDR_RF24_TX_ADR, RF24_TX_ADR, buffEeprom);
    WriteData32ToBuffer(ADDR_RF24_RX0_ADR, RF24_RX0_ADR, buffEeprom);
    WriteData32ToBuffer(ADDR_RF24_RX1_ADR, RF24_RX1_ADR, buffEeprom);
    buffEeprom[ADDR_RF24_RX2_ADR] = RF24_RX2_ADR;
    buffEeprom[ADDR_RF24_RX3_ADR] = RF24_RX3_ADR;
    buffEeprom[ADDR_RF24_RX4_ADR] = RF24_RX4_ADR;
    buffEeprom[ADDR_RF24_RX5_ADR] = RF24_RX5_ADR;
/*----------------------------------------------------------------------------*/
    //ADC
    buffEeprom[ADDR_SENSOR_ON_OFF] = SENSOR_ON_OFF;
    buffEeprom[ADDR_CALIB_SENSOR1] = CALIB_SENSOR1;
    buffEeprom[ADDR_CALIB_SENSOR2] = CALIB_SENSOR2;
    buffEeprom[ADDR_CALIB_SENSOR3] = CALIB_SENSOR3;
    buffEeprom[ADDR_CALIB_SENSOR4] = CALIB_SENSOR4;
    buffEeprom[ADDR_CALIB_PRESSURE1] = CALIB_PRESSURE1;
    buffEeprom[ADDR_CALIB_PRESSURE2] = CALIB_PRESSURE2;
    buffEeprom[ADDR_CALIB_PRESSURE3] = CALIB_PRESSURE3;
    buffEeprom[ADDR_CALIB_PRESSURE4] = CALIB_PRESSURE4;
    buffEeprom[ADDR_ALARM_SENSOR1] = ALARM_SENSOR1;
    buffEeprom[ADDR_ALARM_SENSOR2] = ALARM_SENSOR2;
    buffEeprom[ADDR_ALARM_SENSOR3] = ALARM_SENSOR3;
    buffEeprom[ADDR_ALARM_SENSOR4] = ALARM_SENSOR4;
    
    buffEeprom[ADDR_CALIB_POWER_V] = CALIB_POWER_V;
    //FAN
    buffEeprom[ADDR_FAN_ON_OFF] = FAN_ON_OFF;
    WriteData16ToBuffer(ADDR_SENSOR_INTRV, SENSOR_INTRV, buffEeprom);
    WriteData16ToBuffer(ADDR_MAX_JOB, MAX_JOB, buffEeprom);
    WriteData16ToBuffer(ADDR_INTERVAL_HUM, INTERVAL_HUM, buffEeprom);
    WriteData16ToBuffer(ADDR_DELAY_HUM, DELAY_HUM, buffEeprom);
    WriteData16ToBuffer(ADDR_MAX_TEMPR, MAX_TEMPR, buffEeprom);
    WriteData16ToBuffer(ADDR_MAX_HUM, MAX_HUM, buffEeprom);
    buffEeprom[ADDR_GIST_TEMPR] = GIST_TEMPR;
    buffEeprom[ADDR_GIST_HUM] = GIST_HUM;
    //HEATING
    buffEeprom[ADDR_HEAT_ON_OFF] = HEAT_ON_OFF;
    buffEeprom[ADDR_HEAT_TEMPER_R1] = HEAT_TEMPER_R1;
    buffEeprom[ADDR_HEAT_TEMPER_R2] = HEAT_TEMPER_R2;
    buffEeprom[ADDR_HEAT_TEMPER_R3] = HEAT_TEMPER_R3;
    buffEeprom[ADDR_HEAT_TEMPER_R4] = HEAT_TEMPER_R4;
    buffEeprom[ADDR_HEAT_TEMPER_R5] = HEAT_TEMPER_R5;
    WriteData16ToBuffer(ADDR_HEAT_JOB_DEFAULT_DELAY, HEAT_JOB_DEFAULT_DELAY, buffEeprom);
    WriteData16ToBuffer(ADDR_HEAT_JOB_INTERVAL, HEAT_JOB_INTERVAL, buffEeprom);
    WriteData16ToBuffer(ADDR_HEAT_SENSOR_INTERVAL, HEAT_SENSOR_INTERVAL, buffEeprom);
    WriteData16ToBuffer(ADDR_HEAT_TOP_TEMPERATURE, HEAT_TOP_TEMPERATURE, buffEeprom);
    buffEeprom[ADDR_HEAT_GIST_TEMPERATURE] = HEAT_GIST_TEMPERATURE;
    buffEeprom[ADDR_HEAT_STEP_DELAY] = HEAT_STEP_DELAY;
    buffEeprom[ADDR_HEAT_MAX_DELAY] = HEAT_MAX_DELAY;
    
    
    
    
/*----------------------------------------------------------------------------*/
    Ee24cxxWrite(buffEeprom);
  }
  settings.dateBuild = ReadData32Buffer(ADDR_DATE_BUILD, buffEeprom);
  settings.number = buffEeprom[ADDR_DEVICE_NUMBER];
  settings.type = buffEeprom[ADDR_DEVICE_TYPE];
  settings.rtcCalib = buffEeprom[ADDR_RTC_CALIBRATION];
  
  settings.rotation = buffEeprom[ADDR_LCD_ROTATION];
  switch(settings.rotation){
    case 0x27:                  //Dspl_Rotation_270
    case 0x09:                  //Dspl_Rotation_90
      settings.maxX = 0x01E0; 
      settings.maxY = 0x0140;
    break; 
    default:                    //Dspl_Rotation_0 Rotation_180
      settings.maxX = 0x0140; 
      settings.maxY = 0x01E0;
    break;
  }
  settings.canSpeed = ReadData32Buffer(ADDR_CAN_SPEED, buffEeprom);
  settings.rs485Speed = ReadData16Buffer(ADDR_RS485_SPEED, buffEeprom);
  settings.canDevice = (settings.type << 0x08) | (settings.number << 0x04);
  //RF24L01
  settings.rf24SpeedPower = buffEeprom[ADDR_RF24_SP_PW];
  settings.rf24Ch = buffEeprom[ADDR_RF24_CH];
  settings.rf24Tx = ReadData32Buffer(ADDR_RF24_TX_ADR, buffEeprom);
  settings.rf24Rx0 = ReadData32Buffer(ADDR_RF24_RX0_ADR, buffEeprom);
  settings.rf24Rx1 = ReadData32Buffer(ADDR_RF24_RX1_ADR, buffEeprom);
  settings.rf24Rx2 = buffEeprom[ADDR_RF24_RX2_ADR];
  settings.rf24Rx3 = buffEeprom[ADDR_RF24_RX3_ADR];
  settings.rf24Rx4 = buffEeprom[ADDR_RF24_RX4_ADR];
  settings.rf24Rx5 = buffEeprom[ADDR_RF24_RX5_ADR];
/*----------------------------------------------------------------------------*/
  //ADC
  settings.sensorOnOff = buffEeprom[ADDR_SENSOR_ON_OFF];
  settings.calibSensor1 = buffEeprom[ADDR_CALIB_SENSOR1];
  settings.calibSensor2 = buffEeprom[ADDR_CALIB_SENSOR2];
  settings.calibSensor3 = buffEeprom[ADDR_CALIB_SENSOR3];
  settings.calibSensor4 = buffEeprom[ADDR_CALIB_SENSOR4];
  settings.calibPressure1 = buffEeprom[ADDR_CALIB_PRESSURE1];
  settings.calibPressure2 = buffEeprom[ADDR_CALIB_PRESSURE2];
  settings.calibPressure3 = buffEeprom[ADDR_CALIB_PRESSURE3];
  settings.calibPressure4 = buffEeprom[ADDR_CALIB_PRESSURE4];
  settings.alarmSensor1 = buffEeprom[ADDR_ALARM_SENSOR1];
  settings.alarmSensor2 = buffEeprom[ADDR_ALARM_SENSOR2];
  settings.alarmSensor3 = buffEeprom[ADDR_ALARM_SENSOR3];
  settings.alarmSensor4 = buffEeprom[ADDR_ALARM_SENSOR4];
  
  settings.calibPowerV = buffEeprom[ADDR_CALIB_POWER_V];
  //FAN
  settings.fanOnOff = buffEeprom[ADDR_FAN_ON_OFF];
  settings.fanSensorInterval = ReadData16Buffer(ADDR_SENSOR_INTRV, buffEeprom);
  settings.fanMaxJob = ReadData16Buffer(ADDR_MAX_JOB, buffEeprom);
  settings.fanIntervalHum = ReadData16Buffer(ADDR_INTERVAL_HUM, buffEeprom);
  settings.fanDelayHum = ReadData16Buffer(ADDR_DELAY_HUM, buffEeprom);
  settings.fanMaxTemperature = ReadData16Buffer(ADDR_MAX_TEMPR, buffEeprom);
  settings.fanMaxHumidity = ReadData16Buffer(ADDR_MAX_HUM, buffEeprom);
  settings.fanGistTemperature = buffEeprom[ADDR_GIST_TEMPR];
  settings.fanGistHumidity = buffEeprom[ADDR_GIST_HUM];
  //HEATING
  settings.heatOnOff = buffEeprom[ADDR_HEAT_ON_OFF];
  heating[0x00].temperature = buffEeprom[ADDR_HEAT_TEMPER_R1];
  heating[0x01].temperature = buffEeprom[ADDR_HEAT_TEMPER_R2];
  heating[0x02].temperature = buffEeprom[ADDR_HEAT_TEMPER_R3];
  heating[0x03].temperature = buffEeprom[ADDR_HEAT_TEMPER_R4];
  heating[0x04].temperature = buffEeprom[ADDR_HEAT_TEMPER_R5];
  settings.heatJobDefDelay = ReadData16Buffer(ADDR_HEAT_JOB_DEFAULT_DELAY, buffEeprom);
  settings.heatJobInterval = ReadData16Buffer(ADDR_HEAT_JOB_INTERVAL, buffEeprom);
  settings.heatSensorInterval = ReadData16Buffer(ADDR_HEAT_SENSOR_INTERVAL, buffEeprom);
  settings.heatTopTemperature = ReadData16Buffer(ADDR_HEAT_TOP_TEMPERATURE, buffEeprom);
  settings.heatGistTemperature = buffEeprom[ADDR_HEAT_GIST_TEMPERATURE];
  settings.heatStepDelay = buffEeprom[ADDR_HEAT_STEP_DELAY];
  settings.heatMaxDelay = buffEeprom[ADDR_HEAT_MAX_DELAY];

  

/*----------------------------------------------------------------------------*/
  
  
  
  
  
  
  
  
/*----------------------------------------------------------------------------*/
  
  dht22.humidity = 0xFFFF;
  dht22.temperature = 0xFFFF;
  
  #if defined(DEBUG)
    printf("< OK >    Read configuration\r\n");
  #endif
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
  
  #if defined(DEBUG)
    printf("< OK >    Start Timer 10Hz\r\n\n");
  #endif
  
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
  
  Ee24cxxInit();
  ReadConfig();
  InInit(); 
  GidrolockInit();
  HeatingInit();
  FanInit();
  RtcInit();
  W25QxxInit();
  LcdInit();
  GuiInit();
  ADCInit();
  Ds18b20Init();
  Dht22Init();
  CanInit();
  Rs485Init();
  Nrf24Init();
  BeepInit();
  Timer10Hz();
  
//  W25QxxEraseBlocks();
}
