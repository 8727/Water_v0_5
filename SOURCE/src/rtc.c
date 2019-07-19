#include "rtc.h"

void RTC_IRQHandler(void){
  if(RTC->CRL & RTC_CRL_SECF){
    RTC->CRL &= ~RTC_CRL_SECF;
    if(0x00 != ds18b20Device) Ds18b20Read();
    InCounterWater();
    
    #if defined DEBUG_RTC
      RtcTimeStamp();
      printf("\r\n");
    #endif
  }
}

void RtcTimeStamp(void){
  RtcTypeDef date;
  RtcSecondsToTime(RtcGetSeconds(), &date);
  printf("\t%d.%d.%d %d:%02d:%02d\t", date.day, date.month, date.year, date.hour, date.min, date.sec);
}

void RtcSecondsToTime(uint32_t seconds, RtcTypeDef* unixTime){
  uint32_t a, t;
  char b, c, d;
  t = seconds % SEC_A_DAY;
  a = ((seconds + 43200) / (SEC_A_DAY >> 1)) + (2440587 << 1) + 1;
  a >>= 1;
  unixTime->wday = a % 7;
  a += 32044;
  b = (4 * a + 3) / 146097;
  a = a - (146097 * b) / 4;
  c = (4 * a + 3) / 1461;
  a = a - (1461 * c) / 4;
  d = (5 * a + 2) / 153;
  unixTime->day = a - (153 * d + 2) / 5 + 1;
  unixTime->month = d + 3 - 12 * (d / 10);
  unixTime->year = 100 * b + c - 4800 + (d / 10);
  unixTime->hour = t / 3600;
  unixTime->min = (t % 3600) / 60;
  unixTime->sec = (t % 3600) % 60;
}

uint32_t RtcTimeToSeconds(RtcTypeDef* unixTime){
  uint8_t a, m;
  uint16_t y;
  a = ((14 - unixTime->month) / 12);
  y = unixTime->year + 4800 - a;
  m = unixTime->month + (12 * a) - 3;
  return (((unixTime->day + ((153 * m + 2) / 5) + 365 * y + (y / 4) - (y / 100) + (y / 400) - 32045) - 2440588) * 
    SEC_A_DAY) + unixTime->sec + unixTime->min * 60 + unixTime->hour * 3600;
}

uint32_t RtcGetSeconds(void){
  return (uint32_t)((RTC->CNTH << 0x10) | RTC->CNTL);
}

void RtcSetSeconds(uint32_t seconds){
  PWR->CR |= PWR_CR_DBP;
  while(!(RTC->CRL & RTC_CRL_RTOFF));
  RTC->CRL |= RTC_CRL_CNF;
  RTC->CNTH = seconds >> 0x10;
  RTC->CNTL = seconds;
  RTC->CRL &= ~RTC_CRL_CNF;
  while(!(RTC->CRL & RTC_CRL_RTOFF));
  PWR->CR &= ~PWR_CR_DBP;
}

void RtcInit(void){
  if((RCC->BDCR & RCC_BDCR_RTCEN) != RCC_BDCR_RTCEN){
//    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
//    RCC->APB1ENR |= RCC_APB1ENR_BKPEN;
    PWR->CR |= PWR_CR_DBP;
    RCC->BDCR |= RCC_BDCR_BDRST;
    RCC->BDCR &= ~RCC_BDCR_BDRST;
    RCC->BDCR |= RCC_BDCR_RTCEN;
    RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;
    RCC->BDCR |= RCC_BDCR_LSEON;
    while((RCC->BDCR & RCC_BDCR_LSEON) != RCC_BDCR_LSEON){}
    BKP->RTCCR |= settings.rtcCalib;
    while(!(RTC->CRL & RTC_CRL_RTOFF));
    RTC->CRL |= RTC_CRL_CNF;
    RTC->PRLL = 0x7FFF;
//    BKP->RTCCR |= BKP_RTCCR_CCO;
    RTC->CNTH = settings.dateBuild >> 0x10;
    RTC->CNTL = settings.dateBuild;
    RTC->CRL &= ~RTC_CRL_CNF;
    while(!(RTC->CRL & RTC_CRL_RTOFF));
    RTC->CRL &= (uint16_t) ~RTC_CRL_RSF;
    while((RTC->CRL & RTC_CRL_RSF) != RTC_CRL_RSF){}
    PWR->CR &= ~PWR_CR_DBP;
  }
  RTC->CRH |= RTC_CRH_SECIE;
//  RTC->CRH |= RTC_CRH_ALRIE;
//  RTC->CRH |= RTC_CRH_OWIE;
  NVIC_SetPriority(RTC_IRQn, PRIORITY_RTC);
  NVIC_EnableIRQ(RTC_IRQn);
  
  #if defined DEBUG_RTC
    printf("< OK >    Initialization RTC");
    RtcTimeStamp();
    printf("\r\n");
  #endif
}
