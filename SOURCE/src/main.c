#include "main.h"

#if defined(DEBUG)
  struct __FILE { int handle; /* Add whatever you need here */ };
  FILE __stdout;
  FILE __stdin;

  int fputc(int ch, FILE *f){
    if(DEMCR & TRCENA){
      while(ITM_Port32(0x00) == 0x00);
      ITM_Port8(0x00) = ch;
    }
    return(ch);
  }
#endif

int main(void){
  #if defined(DEBUG)
    printf("Start \n\r");
  #endif
  Setting();
  while(0x01){
  }
}
