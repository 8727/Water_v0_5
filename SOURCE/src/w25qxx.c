#include "w25qxx.h"

struct w25qxxInitTypeDef w25qxx;

uint8_t W25QxxWriteRead(uint8_t byte){
  while(!(SPI1->SR & SPI_SR_TXE));
  SPI1->DR = byte;
  while(!(SPI1->SR & SPI_SR_RXNE));
  return SPI1->DR;
}

void W25QxxWriteWaitEnd(void){
  uint8_t status = 0x00;
  W25Qxx_CS_LOW;
  W25QxxWriteRead(CMD_R_STATUS_1);
  do{
    status = W25QxxWriteRead(0x00);
    #if defined DEBUG_W25QXX
      printf("EEPROM Wait...\r\n");
    #endif
  }while((status & 0x01) == 0x01);
  W25Qxx_CS_HIGHT;
}

void W25QxxWriteOn(void){
  W25Qxx_CS_LOW;
  W25QxxWriteRead(CMD_WRITE_ON);
  W25Qxx_CS_HIGHT;
}

void W25QxxWriteOff(void){
  W25Qxx_CS_LOW;
  W25QxxWriteRead(CMD_WRITE_OFF);
  W25Qxx_CS_HIGHT;
} 

void W25QxxErase(void){
  W25QxxWriteOn();
  W25Qxx_CS_LOW;
  W25QxxWriteRead(CMD_ERASE_CHIP);
  W25Qxx_CS_HIGHT;
  W25QxxWriteWaitEnd();
  W25QxxWriteOff();
}

void W25QxxEraseBlocks(void){
//  GuiEraseW25qxx();
  for(uint8_t i = 0x00; i < w25qxx.blocks; i++){
    W25QxxWriteOn();
    W25Qxx_CS_LOW;
    W25QxxWriteRead(CMD_ERASE_64K);
    W25QxxWriteRead(i);
    W25QxxWriteRead(0x00);
    W25QxxWriteRead(0x00);
    W25Qxx_CS_HIGHT;
//    GuiEraseBlocks(i);
    W25QxxWriteWaitEnd();
    W25QxxWriteOff();
  }
}

void W25QxxReadPage(uint16_t page, uint8_t *buff){
  W25Qxx_CS_LOW;
  W25QxxWriteRead(CMD_FAST_READ);
  W25QxxWriteRead((page & 0xFFFF) >> 0x08);
  W25QxxWriteRead(page & 0x00FF);
  W25QxxWriteRead(0x00);
  W25QxxWriteRead(0x00);
  for(uint16_t i = 0x00; i < 0x0100; i++)
    buff[i] = W25QxxWriteRead(0x00);
  W25Qxx_CS_HIGHT;
}

void W25QxxWritePage(uint16_t page, uint8_t *buff){
  W25QxxWriteOn();
  W25Qxx_CS_LOW;
  W25QxxWriteRead(CMD_WRITE);
  W25QxxWriteRead((page & 0xFFFF) >> 0x08);
  W25QxxWriteRead(page & 0x00FF);
  W25QxxWriteRead(0x00);
  for(uint16_t i = 0x00; i < 0x0100; i++)
    W25QxxWriteRead(buff[i]);
  W25Qxx_CS_HIGHT;
  W25QxxWriteWaitEnd();
  W25QxxWriteOff();
}

void W25QxxReadTable(void){
  uint16_t i;
  W25Qxx_CS_LOW;
  W25QxxWriteRead(CMD_FAST_READ);
  W25QxxWriteRead(0x00);
  W25QxxWriteRead(0x00);
  W25QxxWriteRead(0x00);
  W25QxxWriteRead(0x00);
  for(i = 0x00; i < 0x0100; i++){
    img[i].address = W25QxxWriteRead(0x00) << 0x08;
    img[i].address |= W25QxxWriteRead(0x00);
    img[i].width = W25QxxWriteRead(0x00) << 0x08;
    img[i].width |= W25QxxWriteRead(0x00);
    img[i].height = W25QxxWriteRead(0x00) << 0x08;
    img[i].height |= W25QxxWriteRead(0x00);
    img[i].type = W25QxxWriteRead(0x00);
    img[i].pictures = W25QxxWriteRead(0x00);
    img[i].fps = W25QxxWriteRead(0x00);
    img[i].a = W25QxxWriteRead(0x00);
    img[i].b = W25QxxWriteRead(0x00);
    img[i].c = W25QxxWriteRead(0x00);
  }
  W25Qxx_CS_HIGHT;
}

void W25QxxInit(void){
  GPIOA->CRL &= ~(GPIO_CRL_CNF4 | GPIO_CRL_CNF5 | GPIO_CRL_CNF7);
  GPIOA->CRL |= GPIO_CRL_CNF5_1 | GPIO_CRL_CNF7_1;
  GPIOA->CRL |= GPIO_CRL_MODE4 | GPIO_CRL_MODE5 | GPIO_CRL_MODE7;   // GPIO SPI Flash
  
  RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
  SPI1->CR1 &= ~SPI_CR1_BR;
  SPI1->CR1 &= ~SPI_CR1_CPOL;
  SPI1->CR1 &= ~SPI_CR1_CPHA;
  SPI1->CR1 &= ~SPI_CR1_DFF;
  SPI1->CR1 &= ~SPI_CR1_LSBFIRST;
  SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
  SPI1->CR1 |= SPI_CR1_MSTR;
  SPI1->CR1 |= SPI_CR1_SPE;     // SPI Flash
  
  w25qxx.id = 0x00;
  
  W25Qxx_CS_LOW;
  W25QxxWriteRead(CMD_ON_RESET);
  W25Qxx_CS_HIGHT;
 
  W25Qxx_CS_LOW;
  W25QxxWriteRead(CMD_RESET);
  W25Qxx_CS_HIGHT;

  W25Qxx_CS_LOW;
  W25QxxWriteRead(CMD_JEDEC_ID);
  W25QxxWriteRead(0x00);
  W25QxxWriteRead(0x00);
  w25qxx.id = W25QxxWriteRead(0x00);
  W25Qxx_CS_HIGHT;
  
  switch (w25qxx.id){
    case 0x16:  w25qxx.blocks = 0x40;
                w25qxx.name = "W25Q32BV";
    break;
    case 0x17:  w25qxx.blocks = 0x80;
                w25qxx.name = "W25Q64FV";
    break;
    case 0x18:  w25qxx.blocks = 0xFF;
                w25qxx.name = "W25Q128FV";
    break;
    default:    w25qxx.blocks = 0x00;
                w25qxx.name = "XXXXXXXXX";
    break;
  }
  
  #if defined DEBUG_W25QXX
    printf("< OK >    Initialization W25Qxx\r\n");
  #endif
  #if defined DEBUG_W25QXX
    printf("< OK >    EEPROM : %s\r\n", w25qxx.name);
  #endif
  W25QxxReadTable();
}
