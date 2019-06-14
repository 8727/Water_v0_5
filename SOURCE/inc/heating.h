#ifndef _HEATING_H
#define _HEATING_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"


/* Define --------------------------------------------------------------------*/
#define R1_OPEN                      GPIOE->BSRR = GPIO_BSRR_BR5  // R1
#define R1_CLOSED                    GPIOE->BSRR = GPIO_BSRR_BS5  // R1

#define R2_OPEN                      GPIOE->BSRR = GPIO_BSRR_BR4  // R2
#define R2_CLOSED                    GPIOE->BSRR = GPIO_BSRR_BS4  // R2

#define R3_OPEN                      GPIOE->BSRR = GPIO_BSRR_BR3  // R3
#define R3_CLOSED                    GPIOE->BSRR = GPIO_BSRR_BS3  // R3

#define R4_OPEN                      GPIOE->BSRR = GPIO_BSRR_BR2  // R4
#define R4_CLOSED                    GPIOE->BSRR = GPIO_BSRR_BS2  // R4

#define R5_OPEN                      GPIOE->BSRR = GPIO_BSRR_BR1  // R5
#define R5_CLOSED                    GPIOE->BSRR = GPIO_BSRR_BS1  // R5




void HeatingInit(void);

#endif /* _HEATING_H */
