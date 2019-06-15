#ifndef _FAN_H
#define _FAN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"


/* Define --------------------------------------------------------------------*/
#define FAN_ON                  GPIOE->BSRR = GPIO_BSRR_BR6
#define FAN_OFF                 GPIOE->BSRR = GPIO_BSRR_BS6

void FanInit(void);

#endif /* _FAN_H */