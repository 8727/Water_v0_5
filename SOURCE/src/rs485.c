#include "rs485.h"

struct Rs485InitTypeDef rs485;

void Rs485Sends(uint8_t *str, uint8_t len){
  uint8_t i = 0x00;
  while(len--){
    rs485.txBuff[rs485.txStop++] = str[i++];
  }
  RX485_TX;
  USART1->CR1 |= USART_CR1_TXEIE;
}

void USART1_IRQHandler(void){
  if(USART1->SR & USART_SR_RXNE){
    USART1->SR &= ~USART_SR_RXNE;
    rs485.rxBuff[rs485.rxStop++] = USART1->DR;
  }
  if(USART1->SR & USART_SR_TXE){
    if (rs485.txStart != rs485.txStop){
      USART1->DR = rs485.txBuff[rs485.txStart++];
    }else{
      USART1->CR1 &= ~USART_CR1_TXEIE;
    }
  }
  if(USART1->SR & USART_SR_TC){
    USART1->SR &= ~USART_SR_TC;
    RX485_RX;
  }
}

void Rs485Init(void){
  GPIOA->CRH &= ~(GPIO_CRH_CNF8 | GPIO_CRH_CNF9 | GPIO_CRH_CNF10);
  GPIOA->CRH |= (GPIO_CRH_MODE8 | GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9 | GPIO_CRH_CNF10_0);
  RX485_RX;
  
  RCC->APB2ENR|= RCC_APB2ENR_USART1EN;
  USART1->BRR = settings.rs485Speed;
  USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
  USART1->CR1 |= USART_CR1_RXNEIE;
//  USART1->CR1 |= USART_CR1_IDLEIE;
  USART1->CR1 |= USART_CR1_TCIE;
  USART1->CR1 |= USART_CR1_UE;
  
  NVIC_SetPriority(USART1_IRQn, PRIORITY_RS485);
  NVIC_EnableIRQ(USART1_IRQn);
}
