#include "main.h"


struct __FILE {int handle;};
FILE __stdout;
int fputc(int ch, FILE *f){ if(DEMCR & TRCENA){ while(ITM_Port32(0x00) == 0x00);
  ITM_Port8(0x00) = ch; }
  return(ch);
}

int main(void){
  Setting();
  xTaskCreate(osTimer10Hz,"Timer10Hz", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
  vTaskStartScheduler();
  while(true){
    
  }
}
