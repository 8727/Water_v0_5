#include "ds18b20.h"

uint8_t lastDeviceFlag;
uint8_t lastDiscrepancy;
uint8_t lastFamilyDiscrepancy;
uint8_t ds18b20Device;

struct Ds18b20InitTypeDef ds18b20[DS18B20_MAX_DEVICES];

uint8_t Ds18b20SendReadBit(uint8_t send){
  while(!(USART3->SR & USART_SR_TC)){}
  USART3->DR = send;
  while(!(USART3->SR & USART_SR_RXNE)){}
  return USART3->DR;
}

void Ds18b20SendByte(uint8_t byte){
  uint8_t i;
  for(i = 0x00; i < 0x08; i++){
    if(byte & 0x01){
      Ds18b20SendReadBit(0xFF);
    }else{
      Ds18b20SendReadBit(0x00);
    }
    byte = byte >> 0x01;
  }
}

uint8_t Ds18b20ReadByte(void){
  uint8_t data = 0x00;
  for (uint8_t i = 0x00; i < 0x08; i++){
  data += ((Ds18b20SendReadBit(0xFF) == 0xFF) ? 0x01 : 0x00) << i;
  }
  return data;
}

uint8_t Ds18b20Reset(void){
  uint8_t status;
  USART3->BRR = USART_2_3_SPEED_96;
  status = Ds18b20SendReadBit(0xF0);
  USART3->BRR = USART_2_3_SPEED_1152;
  return status;
}

uint8_t Ds18b20Search(uint8_t *roms){
  uint8_t idBit, cmpIdBit, searchDirection;
  uint8_t idBitNumber = 0x01;
  uint8_t romByteMask = 0x01;
  uint8_t lastZero = 0x00;
  uint8_t romByteNumber = 0x00;
  uint8_t searchResult = 0x00;
  uint8_t romNo[0x08];
  if(!lastDeviceFlag){
    Ds18b20Reset();
    Ds18b20SendByte(DS18B20_SEARCH);
    do{
    idBit =  (Ds18b20SendReadBit(0xFF) == 0xFF) ? 0x01 : 0x00;
    cmpIdBit =  (Ds18b20SendReadBit(0xFF) == 0xFF) ? 0x01 : 0x00;
      if((idBit == 0x01) && (cmpIdBit == 0x01)){
        break;
      }else{
        if(idBit != cmpIdBit){
          searchDirection = idBit;
        }else{
          if(idBitNumber < lastDiscrepancy){
            searchDirection = ((romNo[romByteNumber] & romByteMask) > 0x00);
          }else{
            searchDirection = (idBitNumber == lastDiscrepancy);
          }
          if(searchDirection == 0x00){
            lastZero = idBitNumber;
          }
        }
        if(searchDirection == 0x01){
          romNo[romByteNumber] |= romByteMask;
        }else{
          romNo[romByteNumber] &= ~romByteMask;
        }
        Ds18b20SendReadBit((searchDirection == 0x00) ? 0x00 : 0xFF);
        idBitNumber++;
        romByteMask <<= 0x01;
        if(romByteMask == 0x00){
          romByteNumber++;
          romByteMask = 0x01;
        }
      }
    }while(romByteNumber < 0x08);
    if(!(idBitNumber < 0x41)){
      lastDiscrepancy = lastZero;
      if(lastDiscrepancy == 0x00){
        lastDeviceFlag = 0x01;
      }
      searchResult = 0x01;
      for(uint8_t i = 0x00; i < 0x08; i++) roms[i] = romNo[i];
      ds18b20Device++;
    }
  }
  if(!searchResult){
    lastDiscrepancy = 0x00;
    lastDeviceFlag = 0x00;
    searchResult = 0x00;
  }
  return searchResult;
}

void Ds18b20SearchROM(void){
  uint8_t i;
  uint8_t rom[0x08];
  ds18b20Device = 0x00;
  for(i = 0x00; i < DS18B20_MAX_DEVICES; i++){
    if(Ds18b20Search(rom)){
      memcpy (ds18b20[i].deviceID, rom,sizeof(rom));
      #if (defined DEBUG_DS18B20 || defined INFO)
        printf("Sensor < %d > ID : %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
        i ,rom[0x00], rom[0x01], rom[0x02], rom[0x03], rom[0x04], rom[0x05], rom[0x06], rom[0x07]);
      #endif
    }else break;
  }
}

void Ds18b20Read(void){
  uint16_t temp;
  for(uint8_t i = 0x00; i < ds18b20Device; i++){
    Ds18b20Reset();
    Ds18b20SendByte(DS18B20_MATCH_ROM);
    for(uint8_t x = 0x00; x < 0x08; x++) Ds18b20SendByte(ds18b20[i].deviceID[x]);
    Ds18b20SendByte(DS18B20_READ_SCRATCHPAD);
    temp = (Ds18b20ReadByte() << 0x08) | Ds18b20ReadByte();
    ds18b20[i].temperature = ((temp & 0x0FFF) >> 0x04);
    ds18b20[i].fraction = ((temp & 0x000F) * 0.625);
    #if (defined DEBUG_DS18B20 || defined INFO)
      printf("Sensor < %d > ID : %02X %02X %02X %02X %02X %02X %02X %02X Temper: %d.%02d\r\n",
      i ,ds18b20[i].deviceID[0x00], ds18b20[i].deviceID[0x01], ds18b20[i].deviceID[0x02], ds18b20[i].deviceID[0x03],
      ds18b20[i].deviceID[0x04], ds18b20[i].deviceID[0x05], ds18b20[i].deviceID[0x06], ds18b20[i].deviceID[0x07],
      ds18b20[i].temperature, ds18b20[i].fraction);
    #endif
  }
  Ds18b20Reset();
  Ds18b20SendByte(DS18B20_SKIP_ROM);
  Ds18b20SendByte(DS18B20_CONVERT_TEMPERATURE);
}

void Ds18b20Init(void){
  GPIOB->CRH |= (GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
  
  RCC->APB1ENR|= RCC_APB1ENR_USART3EN;
  
  USART3->BRR = USART_2_3_SPEED_96;
  USART3->CR1 |= USART_CR1_TE | USART_CR1_RE;
  USART3->CR3 |= USART_CR3_HDSEL;
  
  USART3->CR1 |= USART_CR1_UE;
  
  #if defined DEBUG_DS18B20
    printf("< OK >    Initialization DS18B20\r\n");
  #endif
  Ds18b20SearchROM();
  
  if(0x00 != ds18b20Device){
    Ds18b20Reset();
    Ds18b20SendByte(DS18B20_SKIP_ROM);
    Ds18b20SendByte(DS18B20_CONVERT_TEMPERATURE);
  }else{
    #if defined DEBUG_DS18B20
      printf("<ERROR>    DS18B20 No sensors\r\n");
    #endif
  }
}
