#include "gidrolock.h"

void BlockWater(void){
  COLD_CLOSED;
  HOT_CLOSED;
  BOILER_CLOSED;
  HEATING_CLOSED;
  CLOSED;
  #if defined INFO
    printf("\r\n********************************\r\n\r\n********* BLOCK WATER **********\r\n\r\n********************************\r\n");
  #endif
}



void GidrolockInit(void){
  GPIOD->CRL = (GPIOD->CRL & (~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6))) | GPIO_CRL_MODE6; // M1
  GPIOC->CRL = (GPIOC->CRL & (~(GPIO_CRL_MODE7 | GPIO_CRL_CNF7))) | GPIO_CRL_MODE7; // M2
  GPIOC->CRL = (GPIOC->CRL & (~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6))) | GPIO_CRL_MODE6; // M3
  GPIOB->CRH = (GPIOB->CRH & (~(GPIO_CRH_MODE8 | GPIO_CRH_CNF8))) | GPIO_CRH_MODE8; // M4
  GPIOB->CRH = (GPIOB->CRH & (~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9))) | GPIO_CRH_MODE9; // M5
  GPIOE->CRL = (GPIOE->CRL & (~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0))) | GPIO_CRL_MODE0; // M6
  
  
  
  #if defined DEBUG_GIDROLOCK
    printf("< OK >    Initialization CRANES\r\n");
  #endif
}
