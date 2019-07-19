#include "in.h"

struct WaterInitTypeDef water;
struct InInitTypeDef in;

void InCounterWater(void){
  if(!(GPIOB->IDR & GPIO_IDR_IDR4)){ in.cold = true; } // counter cold PB4
  if((GPIOB->IDR & GPIO_IDR_IDR4) && in.cold){ water.coldCounter += WATER_LITERS; in.cold = false; } // counter cold PB4
  if(!(GPIOB->IDR & GPIO_IDR_IDR5)){ in.hot = true; } // counter hot PB5
  if((GPIOB->IDR & GPIO_IDR_IDR5) && in.hot){ water.hotCounter += WATER_LITERS; in.hot = false; } // counter hot PB5
}

void InUpdate10Hz(void){
  if(!(GPIOB->IDR & GPIO_IDR_IDR0)){  }
  if(!(GPIOB->IDR & GPIO_IDR_IDR1)){  }
  if(!(GPIOB->IDR & GPIO_IDR_IDR2)){  }
  if(!(GPIOC->IDR & GPIO_IDR_IDR5)){  }
  
}

void InInit(void){
  GPIOB->BSRR = GPIO_BSRR_BR4; // counter cold PB4
  GPIOB->BSRR = GPIO_BSRR_BR5; // counter hot PB5
  GPIOB->CRL &= ~(GPIO_CRL_MODE0 |GPIO_CRL_MODE1 | GPIO_CRL_MODE2 | GPIO_CRL_MODE4 | GPIO_CRL_MODE5);
  GPIOB->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_CNF1 | GPIO_CRL_CNF2 | GPIO_CRL_CNF4 | GPIO_CRL_CNF5);
  GPIOB->CRL |=  (GPIO_CRL_CNF0_0 | GPIO_CRL_CNF1_0 | GPIO_CRL_CNF2_0 | GPIO_CRL_CNF4_0 | GPIO_CRL_CNF5_0);
  GPIOC->CRL = (GPIOC->CRL & (~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5))) | GPIO_CRL_CNF5_0;
  
  #if defined DEBUG_IN
    printf("< OK >    Initialization PORTS\r\n");
  #endif
}
