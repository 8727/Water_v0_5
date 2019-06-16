#include "dht22.h"

struct Dht22InitTypeDef dht22;
static uint16_t dht22Delay[0x04] = {0xC288, 0xFFFF, 0xFFFF, 0xFFFF};

void Dht22Init(void){
  GPIOD->CRH |= GPIO_CRH_MODE12;
  GPIOD->CRH |= GPIO_CRH_CNF12;
  
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;
  DMA1_Channel7->CCR = 0x00;
  DMA1_Channel7->CMAR = (uint32_t) &dht22Delay[0x00];
  DMA1_Channel7->CPAR = (uint32_t) &TIM4->CCR1;
  DMA1_Channel7->CNDTR = sizeof(dht22Delay)/sizeof(*dht22Delay);
  DMA1_Channel7->CCR = DMA_CCR7_PL_1 | DMA_CCR7_MSIZE_0 | DMA_CCR7_PSIZE_1 | DMA_CCR7_MINC | DMA_CCR7_CIRC | DMA_CCR7_DIR;
  DMA1->IFCR = DMA_IFCR_CGIF7 | DMA_IFCR_CHTIF7 | DMA_IFCR_CTCIF7 | DMA_IFCR_CTEIF7;
  DMA1_Channel7->CCR |= DMA_CCR5_EN;
  
  DMA1_Channel4->CCR = 0x00;
  DMA1_Channel4->CMAR = (uint32_t) &dht22.dataBuff[0x00];
  DMA1_Channel4->CPAR = (uint32_t) &TIM4->CCR2;
  DMA1_Channel4->CNDTR = sizeof(dht22.dataBuff)/sizeof(*dht22.dataBuff);
  DMA1_Channel4->CCR = DMA_CCR4_PL_1 | DMA_CCR4_MSIZE_0 | DMA_CCR4_PSIZE_1 | DMA_CCR4_MINC | DMA_CCR4_TCIE;
  DMA1->IFCR = DMA_IFCR_CGIF4 | DMA_IFCR_CHTIF4 | DMA_IFCR_CTCIF4 | DMA_IFCR_CTEIF4;
  
  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
  TIM4->PSC = 0x031F; //799  настраиваем на период 10 мкс и переполнение на 100мс
  TIM4->ARR = 0xC34F; //49999
  TIM4->SR = 0x00;
  TIM4->DIER = TIM_DIER_UDE | TIM_DIER_CC1IE | TIM_DIER_CC2DE;
  TIM4->CCMR1 = TIM_CCMR1_OC1M | TIM_CCMR1_CC2S_1;
  TIM4->CR1 = TIM_CR1_ARPE;
  TIM4->EGR = TIM_EGR_UG;
  TIM4->CCER = TIM_CCER_CC1E | TIM_CCER_CC1P | TIM_CCER_CC2P;
  TIM4->CR1 |= TIM_CR1_CEN;
  
  NVIC_SetPriority(TIM4_IRQn, PRIORITY_DHT22_TIM);
  NVIC_EnableIRQ(TIM4_IRQn);
  
  NVIC_SetPriority(DMA1_Channel4_IRQn, PRIORITY_DHT22_DMA);
  NVIC_EnableIRQ(DMA1_Channel4_IRQn);
}

void TIM4_IRQHandler(void){
  if(TIM4->SR & TIM_SR_CC1IF){
   DMA1_Channel4->CCR &= ~DMA_CCR4_EN;
   DMA1_Channel4->CMAR = (uint32_t)&dht22.dataBuff[0x00];
   DMA1_Channel4->CNDTR = sizeof(dht22.dataBuff)/sizeof(*dht22.dataBuff);
   DMA1_Channel4->CCR |= DMA_CCR4_EN;
   TIM4->CCER |= TIM_CCER_CC2E;
   TIM4->SR &= ~TIM_SR_CC1IF;
  }
}

void DMA1_Channel4_IRQHandler(void){
  uint64_t tData = 0x00;
  uint8_t *tDataBuff = (uint8_t*)&tData;
  uint8_t a;
  uint32_t *b;
  uint8_t tCRC = 0x00;
  
  if(DMA1->ISR & DMA_ISR_TCIF4){ // приняли данные от DHT22
    TIM4->CCER &= ~TIM_CCER_CC2E;
    b = BITBAND_RAMADR((uint32_t)&tData, 0x00);
    for(a = 0x00; a < 0x28; a++)
    if((dht22.dataBuff[a + 0x02] - dht22.dataBuff[a + 0x01]) > 0x0A) b[0x27 - a] = 0x01;
      else b[0x27 - a] = 0x00;
    tCRC = 0x00;
    for(a = 0x04; a > 0x00; a--) tCRC = tCRC + tDataBuff[a]; // подсчет CRC
    if(tCRC == tDataBuff[0x00]){
      dht22.humidity = (uint16_t)(tData >> 0x18);
      dht22.temperature = (uint16_t)(tData >> 0x08);
    }else{
      dht22.humidity = 0xFFFF;
      dht22.temperature = 0xFFFF;
    }
    DMA1->IFCR = DMA_IFCR_CTCIF4;
  }
}
