#ifndef _FAN_H
#define _FAN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "setting.h"

/* Define --------------------------------------------------------------------*/
#define FAN_ON                  GPIOE->BSRR = GPIO_BSRR_BR6
#define FAN_OFF                 GPIOE->BSRR = GPIO_BSRR_BS6

struct fanInitTypeDef{
  _Bool    job;
  _Bool    status;
  uint16_t jobTimer;
  uint16_t sensorTimer;
  uint16_t timer;
  uint16_t oldTemperature[0x0A];
  uint16_t oldHumidity[0x0A];
};

extern struct fanInitTypeDef fan;

void FanAnalyze(void);
void FanInit(void);

#endif /* _FAN_H */
