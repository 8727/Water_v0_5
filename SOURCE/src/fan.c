#include "fan.h"

void FanInit(void){
  GPIOE->CRL = (GPIOE->CRL & (~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6))) | (GPIO_CRL_MODE6 | GPIO_CRL_CNF6_0); // FAN
  
}
