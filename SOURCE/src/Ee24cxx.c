#include "Ee24cxx.h"

void Ee24cxxRead(uint8_t* data){
  uint8_t i = 0x00;
  uint8_t len = EEPROM_BUFF;
  I2C1->CR1 |= I2C_CR1_ACK;
  I2C1->CR1 |= I2C_CR1_START;
  while (!(I2C1->SR1 & I2C_SR1_SB)){}
  (void) I2C1->SR1;
  I2C1->DR = 0xA0;
  while (!(I2C1->SR1 & I2C_SR1_ADDR)){}
  (void) I2C1->SR1;
  (void) I2C1->SR2;
  I2C1->DR = 0x00;
  while (!(I2C1->SR1 & I2C_SR1_TXE)){}
  I2C1->CR1 |= I2C_CR1_START;
  while (!(I2C1->SR1 & I2C_SR1_SB)){}
  (void) I2C1->SR1;
  I2C1->DR = 0xA1;
  while (!(I2C1->SR1 & I2C_SR1_ADDR)){}
  (void) I2C1->SR1;
  (void) I2C1->SR2;
  if (0x01 < len){
    len--;
    while(len--){
      while (!(I2C1->SR1 & I2C_SR1_RXNE)){}
      data[i++] = I2C1->DR;
    }
  }
  while (!(I2C1->SR1 & I2C_SR1_BTF)){}
  I2C1->CR1 &= ~I2C_CR1_ACK;
  data[i++] = I2C1->DR;
  I2C1->CR1 |= I2C_CR1_STOP;
}

void Ee24cxxWrite(uint8_t* data){
  uint8_t i = 0x00;
  for(uint8_t y = 0x00; y < (EEPROM_BUFF/ 0x08); y++){
    uint8_t len = 0x08;
    //I2C1->CR1 |= I2C_CR1_ACK;
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB)){}
    (void) I2C1->SR1;
    I2C1->DR = 0xA0;
    while (!(I2C1->SR1 & I2C_SR1_ADDR)){}
    (void) I2C1->SR1;
    (void) I2C1->SR2;
    I2C1->DR = i;
    while (len--){
      while (!(I2C1->SR1 & I2C_SR1_TXE)){}
      I2C1->DR = data[i++];
    }
    while (!(I2C1->SR1 & I2C_SR1_BTF)){}
    I2C1->CR1 |= I2C_CR1_STOP;
    DelayMs(0x05);
  }
}

void Ee24cxxInit(void){
  GPIOB->CRL |= (GPIO_CRL_MODE6 | GPIO_CRL_MODE7);
  GPIOB->CRL |= (GPIO_CRL_CNF6 | GPIO_CRL_CNF7);
  
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
  
  I2C1->CR1 = 0x00;
  I2C1->CR2 = 0x00;
  I2C1->CR2 |= 0x28;
//  I2C1->CR2 |= I2C_CR2_ITEVTEN;
  I2C1->TRISE = 0x0D;
  I2C1->OAR1 |= I2C_OAR1_ADD4;
  I2C1->CCR |= I2C_CCR_FS;
  I2C1->CCR |= 0x0022;
  I2C1->CR1 |= I2C_CR1_PE;
  
  #if defined DEBUG_EE24CXX
    printf("< OK >    Initialization EE24Cxx\r\n");
  #endif
}
