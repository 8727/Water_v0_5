#include "rtc.h"

void RTC_IRQHandler(void){
  if(RTC->CRL & RTC_CRL_SECF){
    if(0x00 != ds18b20Device) Ds18b20Read();
    RTC->CRL &= ~RTC_CRL_SECF;
  }
}

void RtcCounterToTime(uint32_t counter, RtcTypeDef* unixTim){
  uint32_t a, t;
  char b, c, d;
  t = counter % SEC_A_DAY;
  a = ((counter + 0xA8C0) / (SEC_A_DAY >> 0x01)) + (0x00254D8B << 0x01) + 0x01;
  a >>= 0x01;
  unixTim->wday = a % 0x07;
  a += 0x7D2C;
  b = (0x04 * a + 0x03) / 0x00023AB1;
  a = a - (0x00023AB1 * b) / 0x04;
  c = (0x04 * a + 0x03) / 0x05B5;
  a = a - (0x05B5 * c) / 0x04;
  d = (0x05 * a + 0x02) / 0x99;
  unixTim->day = a - (0x99 * d + 0x02) / 0x05 + 0x01;
  unixTim->month = d + 0x03 - 0x0C * (d / 0x0A);
  unixTim->year = 0x64 * b + c - 0x12C0 + (d / 0x0A);
  unixTim->hour = t / 0x0E10;
  unixTim->min = (t % 0x0E10) / 0x3C;
  unixTim->sec = (t % 0x0E10) % 0x3C;
}

uint32_t RtcTimeToCounter(RtcTypeDef* unixTime){
  uint8_t a, m;
  uint16_t y;
  a = ((0x0E - unixTime->month) / 0x0C);
  y = unixTime->year + 0x12C0 - a;
  m = unixTime->month + (0x0C * a) - 0x03;
  return (((unixTime->day + ((0x99 * m + 0x02) / 0x05) + 0x016D * y + (y / 0x04) - (y / 0x64) + (y / 0x0190) - 0x7D2D) - 0x00253D8C) * 
          SEC_A_DAY) + unixTime->sec + unixTime->min * 0x3C + unixTime->hour * 0x0E10;
}

uint32_t RtcGetCounter(void){
  return (uint32_t)((RTC->CNTH << 0x10) | RTC->CNTL);
}

void RtcSetCounter(uint32_t counter){
  PWR->CR |= PWR_CR_DBP;
  while(!(RTC->CRL & RTC_CRL_RTOFF));
  RTC->CRL |= RTC_CRL_CNF;
  RTC->CNTH = counter >> 0x10;
  RTC->CNTL = counter;
  RTC->CRL &= ~RTC_CRL_CNF;
  while(!(RTC->CRL & RTC_CRL_RTOFF));
  PWR->CR &= ~PWR_CR_DBP;
}

void RtcInit(void){
  if((RCC->BDCR & RCC_BDCR_RTCEN) != RCC_BDCR_RTCEN){
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    RCC->APB1ENR |= RCC_APB1ENR_BKPEN;
    PWR->CR |= PWR_CR_DBP;
    RCC->BDCR |= RCC_BDCR_BDRST;
    RCC->BDCR &= ~RCC_BDCR_BDRST;
    RCC->BDCR |= RCC_BDCR_RTCEN;
    RCC->BDCR |= RCC_BDCR_RTCSEL_LSE;
    RCC->BDCR |= RCC_BDCR_LSEON;
    while((RCC->BDCR & RCC_BDCR_LSEON) != RCC_BDCR_LSEON){}
    BKP->RTCCR |= settings.rtcCalibration;
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
}
