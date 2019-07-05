#include "nrf24.h"

void Nrf24Init(void){
  
  #if defined(DEBUG)
    if(info){ printf("<ERROR>"); }else{ printf("< OK >"); }
    printf("    Initialization nRF24\r\n");
  #endif
}
