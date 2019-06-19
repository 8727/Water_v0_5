#include "main.h"

#if (defined (DEBUG) || defined(INFO))
  struct __FILE { int handle; };
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
    printf("Start setting\n\r //********************************//\r\n");
  #endif
  Setting();
  while(0x01){
  }
}
