#include "setting.h"

struct settingsInitTypeDef settings;
static __IO uint32_t msTicks;

void SysTick_Handler(void){ msTicks++; }

uint32_t GetTick(void){ return msTicks; }

void DelayMs(uint32_t ms){ uint32_t tickstart = GetTick();
  while((GetTick() - tickstart) < ms){}
}

void DelayMc(uint32_t mc){ mc *= (SystemCoreClock / 1000000) / 9;
  while (mc--);
}

void WriteData32ToBuffer(uint8_t addr, uint32_t data, uint8_t* buff){
  buff[addr + 0x03] = (data >> 0x18);
  buff[addr + 0x02] = (data >> 0x10);
  buff[addr + 0x01] = (data >> 0x08);
  buff[addr] = data; 
}

void WriteData16ToBuffer(uint8_t addr, uint16_t data, uint8_t* buff){
  buff[addr + 0x01] = (data >> 0x08);
  buff[addr] = data;
}

uint32_t ReadData32Buffer(uint8_t addr, uint8_t* buff){
  uint32_t data = buff[addr + 0x03] << 0x18;
  data |= buff[addr + 0x02] << 0x10;
  data |= buff[addr + 0x01] << 0x08;
  data |= buff[addr]; 
  return data;
}

uint16_t ReadData16Buffer(uint8_t addr, uint8_t* buff){
  uint16_t data = buff[addr + 0x01] << 0x08;
  data |= buff[addr];
  return data;
}

void ReadConfig(void){
  #if defined DEBUG_SETTING
    printf("< OK >    Read configuration\r\n");
  #endif
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
    buffEeprom[ADDR_DEVICE_TYPE] = DEVICE_TYPE;
    buffEeprom[ADDR_DEVICE_NUMBER] = DEVICE_NUMBER;
    buffEeprom[ADDR_RTC_CALIBRATION] = RTC_CALIBRATION;
    WriteData32ToBuffer(ADDR_CAN_SPEED, CAN_SPEED, buffEeprom);
    WriteData16ToBuffer(ADDR_RS485_SPEED, RS485_SPEED, buffEeprom);
    
    buffEeprom[ADDR_LCD_ROTATION] = LCD_ROTATION;
    buffEeprom[ADDR_TIME_ZONE] = TIME_ZONE;
    //RF24L01
    WriteData16ToBuffer(ADDR_RF24_ADDR, RF24_ADDR, buffEeprom);
    buffEeprom[ADDR_RF24_PRIM] = RF24_PRIM;
    buffEeprom[ADDR_RF24_SECON] = RF24_SECON;
    buffEeprom[ADDR_RF24_SPEED] = RF24_SPEED;
    buffEeprom[ADDR_RF24_POWER] = RF24_POWER;
    buffEeprom[ADDR_RF24_CH] = RF24_CH;
    buffEeprom[ADDR_RF24_TYPE_ON] = 0x00;
    buffEeprom[ADDR_RF24_TYPE_SEND_1] = 0x00;
    buffEeprom[ADDR_RF24_TYPE_ADDR_1] = 0x00;
    buffEeprom[ADDR_RF24_TYPE_SEND_2] = 0x00;
    buffEeprom[ADDR_RF24_TYPE_ADDR_2] = 0x00;
    buffEeprom[ADDR_RF24_TYPE_SEND_3] = 0x00;
    buffEeprom[ADDR_RF24_TYPE_ADDR_3] = 0x00;
    buffEeprom[ADDR_RF24_TYPE_SEND_4] = 0x00;
    buffEeprom[ADDR_RF24_TYPE_ADDR_4] = 0x00;
    
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
    //WATER
    WriteData32ToBuffer(ADDR_WATER_HOT_COUNTER, WATER_HOT_COUNTER, buffEeprom);
    WriteData32ToBuffer(ADDR_WATER_COLD_COUNTER, WATER_COLD_COUNTER, buffEeprom);
    WriteData16ToBuffer(ADDR_WATER_CALIB_HOT, WATER_CALIB_HOT, buffEeprom);
    WriteData16ToBuffer(ADDR_WATER_CALIB_COLD, WATER_CALIB_COLD, buffEeprom);
    WriteData16ToBuffer(ADDR_WATER_CALIB_OUT_HOT, WATER_CALIB_OUT_HOT, buffEeprom);
    WriteData16ToBuffer(ADDR_WATER_CALIB_OUT_COLD, WATER_CALIB_OUT_COLD, buffEeprom);
    
/*----------------------------------------------------------------------------*/
    Ee24cxxWrite(buffEeprom);
    
    #if defined DEBUG_SETTING
      printf("          Default configuration\r\n");
    #endif
  }
  settings.dateBuild = ReadData32Buffer(ADDR_DATE_BUILD, buffEeprom);
  settings.type = buffEeprom[ADDR_DEVICE_TYPE];
  settings.number = buffEeprom[ADDR_DEVICE_NUMBER];
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
  settings.rf24Addr = ReadData16Buffer(ADDR_RF24_ADDR, buffEeprom);
  settings.rf24Prim = buffEeprom[ADDR_RF24_PRIM];
  settings.rf24Secon = buffEeprom[ADDR_RF24_SECON];
  settings.rf24Speed = buffEeprom[ADDR_RF24_SPEED];
  settings.rf24Power = buffEeprom[ADDR_RF24_POWER];
  settings.rf24Ch = buffEeprom[ADDR_RF24_CH];
  settings.rf24TypeOn = buffEeprom[ADDR_RF24_TYPE_ON];
  settings.rf24TypeSend1 = buffEeprom[ADDR_RF24_TYPE_SEND_1];
  settings.rf24TypeAddr1 = buffEeprom[ADDR_RF24_TYPE_ADDR_1];
  settings.rf24TypeSend2 = buffEeprom[ADDR_RF24_TYPE_SEND_2];
  settings.rf24TypeAddr2 = buffEeprom[ADDR_RF24_TYPE_ADDR_2];
  settings.rf24TypeSend3 = buffEeprom[ADDR_RF24_TYPE_SEND_3];
  settings.rf24TypeAddr3 = buffEeprom[ADDR_RF24_TYPE_ADDR_3];
  settings.rf24TypeSend4 = buffEeprom[ADDR_RF24_TYPE_SEND_4];
  settings.rf24TypeAddr4 = buffEeprom[ADDR_RF24_TYPE_ADDR_4];
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
  //WATER
  water.hotCounter = ReadData32Buffer(ADDR_WATER_HOT_COUNTER, buffEeprom);
  water.coldCounter = ReadData32Buffer(ADDR_WATER_COLD_COUNTER, buffEeprom);
  water.calibHot = ReadData16Buffer(ADDR_WATER_CALIB_HOT, buffEeprom);
  water.calibCold = ReadData16Buffer(ADDR_WATER_CALIB_COLD, buffEeprom);
  water.calibOutHot = ReadData16Buffer(ADDR_WATER_CALIB_OUT_HOT, buffEeprom);
  water.calibOutCold = ReadData16Buffer(ADDR_WATER_CALIB_OUT_COLD, buffEeprom);
  
  
  
  
  
  
  
  
/*----------------------------------------------------------------------------*/
  
  dht22.humidity = 0xFFFF;
  dht22.temperature = 0xFFFF;
}

void TIM6_IRQHandler(void){
  HeatingPWM();
  FanAnalyze();
  ADCAlarm();
  InUpdate10Hz();
  TIM6->SR &= ~TIM_SR_UIF;
}

void EXTI15_10_IRQHandler(void){
  if((EXTI->PR & EXTI_PR_PR13) == EXTI_PR_PR13){ // Прерывание от EXTI13 //PD13
    Nrf24CheckRadio();
    EXTI->PR |= EXTI_PR_PR13; // Сбросить флаг EXTI13
  }
  
  
  
}

void Timer10Hz(void){  
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
  TIM6->PSC = 0x1F3F; // 7999 80000000:8000=10000Hz
  TIM6->ARR = 0x03E7; // 10Hz
  TIM6->SR = 0x00;
  TIM6->DIER |= TIM_DIER_UIE;
  TIM6->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE;
  
  #if defined DEBUG_SETTING
    printf("< OK >    Start Timer 10Hz\r\n\n");
  #endif
  
  NVIC_SetPriority(TIM6_IRQn, PRIORITY_HEATING);
  NVIC_EnableIRQ(TIM6_IRQn);
}

void Setting(void){
  #if defined DEBUG_SETTING
    printf("\t\tStart setting\n\r\n");
  #endif
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
  #if defined DEBUG_SETTING
    printf("\t\tStop setting\n\r\n");
  #endif
  
//  W25QxxEraseBlocks();
}
