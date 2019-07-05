#include "beep.h"


void BeepInit(void){
  GPIOC->CRH |= GPIO_CRH_MODE13;
  GPIOC->CRH &= ~(GPIO_CRH_CNF13);
  
  #if defined(DEBUG)
    printf("< OK >    Initialization BEEP\r\n");
  #endif
}
