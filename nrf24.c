#include "nrf24.h"

uint8_t Nrf24WriteRead(uint8_t byte){
  while(!(SPI2->SR & SPI_SR_TXE));
  SPI2->DR = byte;
  while(!(SPI2->SR & SPI_SR_RXNE));
  return SPI2->DR;
}

uint8_t Nrf24ReadReg(uint8_t addr){
  uint8_t byte = 0x00;
  NRF24_CS_LOW;
  byte = Nrf24WriteRead(addr);
  if(addr != NRF24_STATUS){
    byte = Nrf24WriteRead(0xFF);
  }
  NRF24_CS_HIGHT;
  return byte;
}

void Nrf24WriteReg(uint8_t addr, uint8_t byte){
  addr |= NRF24_W_REGISTER;
  NRF24_CS_LOW;
  Nrf24WriteRead(addr);
  Nrf24WriteRead(byte);
  NRF24_CS_HIGHT;
}

void Nrf24ReadBuff(uint8_t addr, uint8_t *buff, uint8_t bytes){
  NRF24_CS_LOW;
  Nrf24WriteRead(addr); //отправим адрес в шину
  for(uint8_t i = 0x00; i < bytes; i++){
    buff[i] = Nrf24WriteRead(0x00);; //отправим данные в буфер
  }
  NRF24_CS_HIGHT;
}

void Nrf24WriteBuff(uint8_t addr, uint8_t *buff, uint8_t bytes){
  addr |= NRF24_W_REGISTER; //включим бит записи в адрес
  NRF24_CS_LOW;
  Nrf24WriteRead(addr); //отправим адрес в шину
  DelayMc(0x01);
  for(uint8_t i = 0x00; i < bytes; i++){
    Nrf24WriteRead(buff[i]); //отправим данные в буфер
  }
  NRF24_CS_HIGHT;
}

void Nrf24FlushRX(void){
  NRF24_CS_LOW;
  Nrf24WriteRead(NRF24_FLUSH_RX);
  DelayMc(0x01);
  NRF24_CS_HIGHT;
}

void Nrf24FlushTX(void){
  NRF24_CS_LOW;
  Nrf24WriteRead(NRF24_FLUSH_TX);
  DelayMc(0x01);
  NRF24_CS_HIGHT;
}
void Nrf24RxMode(void){
  uint8_t regval=0x00;
  regval = Nrf24ReadReg(NRF24_CONFIG);
  //разбудим модуль и переведём его в режим приёмника, включив биты PWR_UP и PRIM_RX
  regval |= (0x01 << NRF24_PWR_UP) | (0x01 << NRF24_PRIM_RX);
  Nrf24WriteReg(NRF24_CONFIG, regval);
  NRF24_CE_HIGHT;
  DelayMc(150); //Задержка минимум 130 мкс
  // Flush buffers
  Nrf24FlushRX();
  Nrf24FlushTX();
}
void Nrf24Init(void){
  uint8_t buff[0x04];
  GPIOB->CRH &= ~(GPIO_CRH_CNF11 | GPIO_CRH_CNF12 | GPIO_CRH_CNF13 | GPIO_CRH_CNF15);
  GPIOB->CRH |= GPIO_CRH_CNF13_1 | GPIO_CRH_CNF15_1;
  GPIOB->CRH |= GPIO_CRH_MODE11 | GPIO_CRH_MODE12 | GPIO_CRH_MODE13 | GPIO_CRH_MODE15;
  
  RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
  SPI2->CR1 &= ~SPI_CR1_BR; // 40MHz
//  SPI2->CR1 |= SPI_CR1_BR_0; // 10MHz
  SPI2->CR1 &= ~SPI_CR1_CPOL;
  SPI2->CR1 &= ~SPI_CR1_CPHA;
  SPI2->CR1 &= ~SPI_CR1_DFF;
  SPI2->CR1 &= ~SPI_CR1_LSBFIRST;
  SPI2->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
  SPI2->CR1 |= SPI_CR1_MSTR;
  SPI2->CR1 |= SPI_CR1_SPE;
  
  
  
  
  
  
  
  
  
//  NRF24_CE_LOW;
//  DelayMs(0x05);
//  Nrf24WriteReg(NRF24_CONFIG, 0x0E);  // Set PWR_UP bit, enable CRC(2 byte) &Prim_RX:0 (Transmitter)
//  DelayMs(0x05);
//  Nrf24WriteReg(NRF24_EN_AA, 0x01); // Enable Pipe1
//  Nrf24WriteReg(NRF24_EN_RXADDR, 0x01); // Enable Pipe1
//  Nrf24WriteReg(NRF24_SETUP_AW, 0x02); // Setup address width=4 bytes
//  Nrf24WriteReg(NRF24_SETUP_RETR, 0x3F); // 1ms, 15 retrans
//  NRF24_CS_LOW;
//  Nrf24WriteRead(NRF24_ACTIVATE);
//  DelayMc(0x01);
//  Nrf24WriteRead(0x73);
//  NRF24_CS_HIGHT;
//  Nrf24WriteReg(NRF24_FEATURE, 0x06);
//  Nrf24WriteReg(NRF24_DYNPD, 0x01);
//  Nrf24WriteReg(NRF24_STATUS, 0x70); //Reset flags for IRQ
//  Nrf24WriteReg(NRF24_CH, settings.rf24Ch); // частота 2420 MHz 
//  Nrf24WriteReg(NRF24_SETUP, 0x06); //TX_PWR:0dBm, Datarate:1Mbps
//  WriteData32ToBuffer(0x00, settings.rf24Pr, buff);
//  Nrf24WriteBuff(NRF24_TX_ADDR, buff, 0x04);
//  Nrf24WriteBuff(NRF24_RX_ADDR_P0, buff, 0x04);
//  WriteData32ToBuffer(0x00, settings.rf24Sc, buff);
//  Nrf24WriteBuff(NRF24_RX_ADDR_P1, buff, 0x04);
//  //Nrf24WriteReg(NRF24_RX_PW_P0, 32); //Number of bytes in RX payload in data pipe 0
  #if defined DEBUG_NRF24
    uint8_t temp = 0x00;
    
    temp = Nrf24ReadReg(NRF24_CONFIG); printf("\tCONFIG:\t 0x%02X\r\n",temp);
    temp = Nrf24ReadReg(NRF24_EN_AA); printf("\tEN_AA:\t 0x%02X\r\n",temp);
    temp = Nrf24ReadReg(NRF24_EN_RXADDR); printf("\tEN_RXADDR:\t 0x%02X\r\n",temp);
    temp = Nrf24ReadReg(NRF24_STATUS); printf("\tSTATUS:\t 0x%02X\r\n",temp);
    temp = Nrf24ReadReg(NRF24_SETUP); printf("\tNRF24_SETUP:\t 0x%02X\r\n",temp);
    Nrf24ReadBuff(NRF24_TX_ADDR, buff, 0x04); printf("\tRF24_ADDR_TX:\t 0x%02X, 0x%02X, 0x%02X, 0x%02X\r\n", buff[0], buff[1], buff[2] ,buff[3]);
    Nrf24ReadBuff(NRF24_RX_ADDR_P0, buff, 0x04); printf("\tRF24_ADDR_RX:\t 0x%02X, 0x%02X, 0x%02X, 0x%02X\r\n", buff[0], buff[1], buff[2], buff[3]);
    Nrf24ReadBuff(NRF24_RX_ADDR_P1, buff, 0x04); printf("\tRF24_ADDR_SC:\t 0x%02X, 0x%02X, 0x%02X, 0x%02X\r\n", buff[0], buff[1], buff[2], buff[3]);



  
//  #if defined DEBUG_NRF24
    printf("< OK >    Initialization nRF24\r\n");
  #endif
}
