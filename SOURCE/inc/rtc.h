#ifndef _RTC_H
#define _RTC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "setting.h"


/* Define --------------------------------------------------------------------*/
#define SEC_A_DAY       0x00015180

typedef struct{
  uint16_t year;    /* 1970..2106 */
  uint8_t  month;   /* 1..12 */
  uint8_t  day;     /* 1..31 */
  uint8_t  wday;    /* 0..6 (Sun..Sat) */
  uint8_t  hour;    /* 0..23 */
  uint8_t  min;     /* 0..59 */
  uint8_t  sec;     /* 0..59 */
}RtcTypeDef;


void RtcSecondsToTime (uint32_t counter, RtcTypeDef* unixTime);
uint32_t RtcTimeToSeconds(RtcTypeDef* unixTime);
uint32_t RtcGetSeconds(void);
void RtcSetSeconds(uint32_t count);
void RtcInit(void);

#endif /* _RTC_H */
