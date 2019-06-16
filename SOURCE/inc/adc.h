#ifndef _ADC_H
#define _ADC_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "setting.h"

/* Define --------------------------------------------------------------------*/

struct AdcInitTypeDef{
  uint8_t  sensor1;
  uint8_t  sensor2;
  uint8_t  sensor3;
  uint8_t  sensor4;
  uint16_t sensor;
  uint16_t pressure1;
  uint16_t pressure2;
  uint16_t pressure3;
  uint16_t pressure4;
  uint8_t  powerV;
  uint8_t  ch;
};

extern struct AdcInitTypeDef adc;

void ADCAlarm(void);
void ADCInit(void);

#endif /* _ADC_H */
