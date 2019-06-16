#ifndef _FAN_H
#define _FAN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "setting.h"

/* Define --------------------------------------------------------------------*/
#define FAN_ON                  GPIOE->BSRR = GPIO_BSRR_BR6
#define FAN_OFF                 GPIOE->BSRR = GPIO_BSRR_BS6

struct fanInitTypeDef{
  uint8_t  status;
  uint16_t sensorCounter;         // 
  uint16_t sensorInterval;        // 
  uint16_t counter;               // 
  uint16_t interval;              // общее время работы 15 мин.
  uint16_t delay;                 // 
  uint16_t oldTemperature[0x0A];  // 
  uint16_t oldHumidity[0x0A];     // 
  uint16_t maxTemperature;        // 
  uint16_t maxHumidity;           // 
  uint8_t  gistTemperature;       // 
  uint8_t  gistHumidity;          // 
};

extern struct fanInitTypeDef fan;
void FanAnalyze(void);
void FanInit(void);

#endif /* _FAN_H */
