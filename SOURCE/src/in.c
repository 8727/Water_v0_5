#include "in.h"

void InInit(void){
  
  #if defined(DEBUG)
    if(info){ printf("<ERROR>"); }else{ printf("< OK >"); }
    printf("    Initialization PORTS\r\n");
  #endif
}
