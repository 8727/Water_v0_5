#ifndef _GIDROLOCK_H
#define _GIDROLOCK_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"



/* Define --------------------------------------------------------------------*/
#define COLD_CLOSED                         GPIOD->BSRR = GPIO_BSRR_BR6  // M1
#define COLD_OPEN                           GPIOD->BSRR = GPIO_BSRR_BS6  // M1

#define HOT_CLOSED                          GPIOC->BSRR = GPIO_BSRR_BR7  // M2
#define HOT_OPEN                            GPIOC->BSRR = GPIO_BSRR_BS7  // M2

#define BOILER_CLOSED                       GPIOC->BSRR = GPIO_BSRR_BR6  // M3
#define BOILER_OPEN                         GPIOC->BSRR = GPIO_BSRR_BS6  // M3

#define DIRECTLY_CLOSED                     GPIOB->BSRR = GPIO_BSRR_BR8  // M4
#define DIRECTLY_OPEN                       GPIOB->BSRR = GPIO_BSRR_BS8  // M4

#define HEATING_CLOSED                      GPIOB->BSRR = GPIO_BSRR_BR9  // M5
#define HEATING_OPEN                        GPIOB->BSRR = GPIO_BSRR_BS9  // M5

#define CLOSED                              GPIOE->BSRR = GPIO_BSRR_BR0  // M6
#define OPEN                                GPIOE->BSRR = GPIO_BSRR_BS0  // M6


void BlockWater(void);
void GidrolockInit(void);
  
#endif /* _GIDROLOCK_H */
