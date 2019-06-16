#include "beep.h"


void BeepInit(void){
  GPIOC->CRH |= GPIO_CRH_MODE13;
  GPIOC->CRH &= ~(GPIO_CRH_CNF13);
}
