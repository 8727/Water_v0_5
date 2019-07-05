#ifndef _BEEP_H
#define _BEEP_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "setting.h"

/* Define --------------------------------------------------------------------*/
#define BEEP_OFF                  GPIOC->BSRR = GPIO_BSRR_BR13
#define BEEP_ON                   GPIOC->BSRR = GPIO_BSRR_BS13


void BeepInit(void);

#endif /* _BEEP_H */
